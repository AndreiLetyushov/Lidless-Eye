#ifndef USBCAMERA_H
#define USBCAMERA_H

#include "abstractcamera.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv/cv.h"

class UsbCamera : public AbstractCamera
{
    Q_OBJECT
public:
    explicit UsbCamera(QObject *parent = 0);
    ~UsbCamera() override;

public slots:
    void initCamera(int device) override;
    void requestNewImage() override;

signals:

private:
    void getVideoProperties();

    cv::VideoCapture *m_capture;
};

#endif // USBCAMERA_H
