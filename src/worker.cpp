#include "worker.h"

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

void Worker::proceedImageFromCam()
{

}

