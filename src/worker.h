#ifndef WORKER_H
#define WORKER_H

#include "usbcamera.h"

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QImage>


class Worker : public QObject
{
    Q_OBJECT
public:
    static const Worker &instance();

signals:
    void getImageFromCam();
    void newImageIsReagy(const QImage &img);

private slots:
    void cameraIsReady(int result);
    void proceedImageFromCam();

private:
    Worker();

    Q_DISABLE_COPY(Worker)

    UsbCamera *m_usbCam;
};

#endif // WORKER_H
