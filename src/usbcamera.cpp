#include "usbcamera.h"


UsbCamera::UsbCamera(QObject *parent) :
    AbstractCamera(parent)
{
}

UsbCamera::~UsbCamera()
{
    m_capture->release();
}

void UsbCamera::initCamera(int device)
{
    m_capture = new cv::VideoCapture(device);

    m_capture->set(CV_CAP_PROP_FRAME_WIDTH, 640);
    m_capture->set(CV_CAP_PROP_FRAME_HEIGHT, 480);

    if(m_capture->isOpened()) {

        emit initCameraFinished(0);
        return;
    }

    emit initCameraFinished(-1);
}

void UsbCamera::requestNewImage()
{
    cv::Mat mat, temp;
    m_capture->read(mat);

    cv::cvtColor(mat, temp, CV_BGR2RGB);

    QImage img((const uchar *)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);


//    mat.release();
//    temp.release();

    emit newImageIsReagy(img);
    return;

}

void UsbCamera::getVideoProperties()
{
    qDebug() << m_capture->get(CV_CAP_PROP_POS_MSEC) << endl;
}


