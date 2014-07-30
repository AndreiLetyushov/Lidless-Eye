#include "worker.h"

#include "proofnetwork/restclient.h"
#include "proofnetwork/profit/presssheetsapi.h"
#include "proofnetwork/profit/presssheetitemsapi.h"
#include "proofnetwork/profit/ordersapi.h"
#include "proofnetwork/profit/usersapi.h"
#include "proofnetwork/profit/data/profituser.h"
#include "proofnetwork/profit/data/presssheet.h"
#include "proofnetwork/profit/data/presssheetitem.h"
#include "proofnetwork/profit/data/order.h"
#include "proofnetwork/profit/data/orderitem.h"
#include "proofnetwork/profit/data/presssheetsearchoptions.h"
#include "proofnetwork/profit/data/workflowelement.h"
#include "proofnetwork/profit/data/transition.h"
#include "proofcore/taskchain.h"
#include "proofcore/settings.h"
#include "proofcore/settingsgroup.h"

#include "proofnetwork/profit/proofnetworkprofit_types.h"
#include "proofnetwork/profit/apihelper.h"

#include <QDebug>
#include <QThread>

const Worker &Worker::instance()
{
    static Worker workerInstance;
    return workerInstance;
}


Worker::Worker()
{
    m_usbCam = new UsbCamera();

    connect(m_usbCam, &UsbCamera::initCameraFinished, this, &Worker::cameraIsReady);
    connect(m_usbCam, &UsbCamera::newImageIsReagy, this, &Worker::newImageIsReagy);

    m_settings = new Proof::Settings(this);

    Proof::SettingsGroup *profitGroup = m_settings->group("profit", true);
    Proof::SettingsGroup *machineGroup = m_settings->group("machine", true);

    QString profitUserName = profitGroup->value("username", "").toString();
    m_operationName = machineGroup->value("operation", "mounting", true).toString();
    m_operation = Proof::Profit::ApiHelper::workflowActionFromString(m_operationName);

    m_restClient = Proof::RestClientSP::create();
    m_restClient->setAuthType(Proof::RestClient::AuthType::WsseAuth);
    m_restClient->setScheme("https");
    m_restClient->setHost(profitGroup->value("host", "profit-qa2.printfulfillmentservices.com", true).toString());
    m_restClient->setClientName(profitGroup->value("clientname", "ProofStation-SingleOperation", true).toString());
    if (!profitUserName.isEmpty())
        m_restClient->setUserName(profitUserName);

    m_settings->sync();

    m_thread = new QThread(this);
    m_pressSheetsApi = new Proof::Profit::PressSheetsApi(m_restClient);
    m_pressSheetItemsApi = new Proof::Profit::PressSheetItemsApi(m_restClient);
    m_usersApi = new Proof::Profit::UsersApi(m_restClient);

    m_pressSheetsApi->moveToThread(m_thread);
    m_pressSheetItemsApi->moveToThread(m_thread);
    m_usersApi->moveToThread(m_thread);

    m_restClient->moveToThread(m_thread);

    m_thread->start();



    m_usbCam->initCamera(0);
}


void Worker::cameraIsReady(int result)
{
    if(result != 0) {
        qDebug() << "Error: Camera initialisation failed : " << result << endl;
        return;
    }

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, m_usbCam, &UsbCamera::requestNewImage);
    timer->start(100);
}


void Worker::login(const QString &username, const QString &password)
{
    Proof::TaskChainSP taskChain = Proof::TaskChain::createChain();
    auto task = [taskChain, this, username, password]() {

        m_restClient->setUserName(username);
        m_restClient->setPassword(password);

        qulonglong currentOperationId = 0;
        QString result;

        std::function<bool(qulonglong, Proof::Profit::ProfitUserSP)> userCallback
                = [&result, &currentOperationId]
                (qulonglong operationId, Proof::Profit::ProfitUserSP user) {
            if (currentOperationId != operationId)
                return false;
            result = user->fullName();
            return true;
        };

        QString errorMessage;

        std::function<bool(qulonglong, Proof::AbstractRestApi::ErrorLevel, qlonglong, const QString&)> errorCallback
                = [&errorMessage, &currentOperationId]
                (qulonglong operationId, Proof::AbstractRestApi::ErrorLevel errorLevel, qlonglong errorNumber, const QString &errorString) {
            Q_UNUSED(errorLevel);
            if (currentOperationId != operationId)
                return false;
            if (errorNumber == 401)
                errorMessage = QString("%1: Wrong username or password (%2)").arg(errorNumber).arg(errorString);
            else
                errorMessage = QString("%1: %2").arg(errorNumber).arg(errorString);
            return true;
        };

        taskChain->addSignalWaiter(m_usersApi, &Proof::AbstractRestApi::errorOccurred, errorCallback);
        taskChain->addSignalWaiter(m_usersApi, &Proof::Profit::UsersApi::userFetched, userCallback);

        if (password.isEmpty())
            currentOperationId = m_usersApi->fetchUserByBarcode(username);
        else
            currentOperationId = m_usersApi->fetchUser(username);

        taskChain->fireSignalWaiters();

        if (result.isEmpty()) {
            emit errorOccured(errorMessage);
        } else {
            setFullUserName(result);
            Proof::SettingsGroup *profitGroup = m_settings->group("profit", true);
            profitGroup->setValue("username", username);
            emit loggedIn();
        }
    };

    taskChain->addTask(task);
}


void Worker::fetchPressSheets()
{
    Proof::TaskChainSP taskChain = Proof::TaskChain::createChain();

    auto task = [taskChain, this]() {

        qulonglong currentOperationId = 0;
        QList<Proof::Profit::PressSheetSP> result;

        std::function<bool(qulonglong, QList<Proof::Profit::PressSheetSP>)> pressSheetsCallback
                = [&result, &currentOperationId]
                (qulonglong operationId, QList<Proof::Profit::PressSheetSP> pressSheets) {
            if (currentOperationId != operationId)
                return false;
            result = pressSheets;
            return true;
        };

        QString errorMessage;

        std::function<bool(qulonglong, Proof::AbstractRestApi::ErrorLevel, qlonglong, const QString&)> errorCallback
                = [&errorMessage, &currentOperationId]
                (qulonglong operationId, Proof::AbstractRestApi::ErrorLevel errorLevel, qlonglong errorNumber, const QString &errorString) {
            Q_UNUSED(errorLevel);
            if (currentOperationId != operationId)
                return false;
            errorMessage = QString("%1: %2").arg(errorNumber).arg(errorString);
            return true;
        };

        taskChain->addSignalWaiter(m_pressSheetsApi, &Proof::AbstractRestApi::errorOccurred, errorCallback);
        taskChain->addSignalWaiter(m_pressSheetsApi, &Proof::Profit::PressSheetsApi::pressSheetsFound, pressSheetsCallback);

        auto searchOptions = Proof::Profit::PressSheetSearchOptionsSP::create();

        searchOptions->setMachine(m_machineName);
        searchOptions->setStatus(Proof::Profit::ApiHelper::ValidEntity);
        searchOptions->addWorkflowElement(new Proof::Profit::WorkflowElement(m_operation, Proof::Profit::ApiHelper::IsReadyForStatus));
        searchOptions->addWorkflowElement(new Proof::Profit::WorkflowElement(m_operation, Proof::Profit::ApiHelper::InProgressStatus));
        searchOptions->addWorkflowElement(new Proof::Profit::WorkflowElement(m_operation, Proof::Profit::ApiHelper::SuspendedStatus));
        currentOperationId = m_pressSheetsApi->findPressSheets(searchOptions);

        taskChain->fireSignalWaiters();

//        if (errorMessage.isEmpty())
//            QMetaObject::invokeMethod(m_model, "setPressSheets", Qt::QueuedConnection, Q_ARG(QList<Proof::Profit::PressSheetSP>, result));
//        else
//            emit errorOccured(errorMessage);
    };
    taskChain->addTask(task);
}


void Worker::fetchPressSheet(qlonglong id)
{
    Proof::TaskChainSP taskChain = Proof::TaskChain::createChain();

    auto task = [taskChain, this, id]() {
        qulonglong currentOperationId = 0;
        Proof::Profit::PressSheetSP result;
        std::function<bool(qulonglong, Proof::Profit::PressSheetSP)> pressSheetCallback
                = [&result, &currentOperationId]
                (qulonglong operationId, Proof::Profit::PressSheetSP pressSheet) {
            if (currentOperationId != operationId)
                return false;
            result = pressSheet;
            return true;
        };

        QString errorMessage;
        std::function<bool(qulonglong, Proof::AbstractRestApi::ErrorLevel, qlonglong, const QString&)> errorCallback
                = [&currentOperationId, &errorMessage]
                (qulonglong operationId, Proof::AbstractRestApi::ErrorLevel errorLevel, qlonglong errorNumber, const QString &errorString) {
            Q_UNUSED(errorLevel);
            if (currentOperationId != operationId)
                return false;
            errorMessage = QString("%1: %2").arg(errorNumber).arg(errorString);
            return true;
        };

        taskChain->addSignalWaiter(m_pressSheetsApi, &Proof::AbstractRestApi::errorOccurred, errorCallback);
        taskChain->addSignalWaiter(m_pressSheetsApi, &Proof::Profit::PressSheetsApi::pressSheetFetched, pressSheetCallback);

        currentOperationId = m_pressSheetsApi->fetchPressSheet(id);
        taskChain->fireSignalWaiters();

        if (result) {
            //Proof::Profit::PressSheetQmlWrapper *wrapper = result->toQmlWrapper();
            //wrapper->moveToThread(qApp->thread());
            //We need to set ownership explicitly here
            //QQmlEngine::setObjectOwnership(wrapper, QQmlEngine::JavaScriptOwnership);
            //emit pressSheetFetched(wrapper);
            qDebug() << "Presssheet was get";
        } else {
            qDebug() << "Presssheet wasn't get";
            //emit errorOccured(errorMessage);
        }
    };
    taskChain->addTask(task);
}


void Worker::setFullUserName(const QString &arg)
{
    if (m_fullUserName != arg) {
        m_fullUserName = arg;
        emit fullUserNameChanged(m_fullUserName);
    }
}


void Worker::proceedImageFromCam()
{

}

