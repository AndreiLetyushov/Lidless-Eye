#ifndef WORKER_H
#define WORKER_H

#include "usbcamera.h"

#include <QObject>

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

private:
    Worker();
    UsbCamera *m_usbCam;
};

#endif // WORKER_H
