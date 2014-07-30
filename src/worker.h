#ifndef WORKER_H
#define WORKER_H

#include "proofnetwork/proofnetwork_types.h"
#include "proofnetwork/profit/apihelper.h"

#include "usbcamera.h"

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QImage>


namespace Proof {
class Settings;
namespace Profit {
class PressSheetQmlWrapper;
class PressSheetsApi;
class PressSheetItemsApi;
class OrdersApi;
class UsersApi;
}
}

class Worker : public QObject
{
    Q_OBJECT
public:
    static const Worker &instance();

public slots:
    void login(const QString &username, const QString &password);
    void fetchPressSheets();
    void fetchPressSheet(qlonglong id);

signals:
    void getImageFromCam();
    void newImageIsReagy(const QImage &img);

    void fullUserNameChanged(const QString &arg);
    void loggedIn();
    void errorOccured(const QString &errorMessage);
    void pressSheetFetched(Proof::Profit::PressSheetQmlWrapper *pressSheet);
    void pressSheetTransitionDone(qlonglong pressSheetId, Proof::Profit::ApiHelper::TransitionEvent event);

private slots:
    void cameraIsReady(int result);
    void proceedImageFromCam();

private:
    void setFullUserName(const QString &arg);

    Worker();

    Q_DISABLE_COPY(Worker)

    UsbCamera *m_usbCam;
    QThread *m_thread;
    Proof::Profit::PressSheetsApi *m_pressSheetsApi;
    Proof::Profit::PressSheetItemsApi *m_pressSheetItemsApi;
    Proof::Profit::UsersApi *m_usersApi;
    Proof::RestClientSP m_restClient;
    QString m_fullUserName;
    bool m_jobListNeeded;

    Proof::Settings *m_settings;

    QString m_machineName;
    QString m_operationName;
    Proof::Profit::ApiHelper::WorkflowAction m_operation;

    //PressSheetsModel *m_model;
};

#endif // WORKER_H
