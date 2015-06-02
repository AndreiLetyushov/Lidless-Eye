#include "usbcamera.h"

#include <QDebug>
#include <QImage>

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

    getVideoProperties();

    m_capture->set(CV_CAP_PROP_FRAME_WIDTH, 1280);
    m_capture->set(CV_CAP_PROP_FRAME_HEIGHT, 960);

    getVideoProperties();

    if (m_capture->isOpened()) {
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

    emit newImageIsReagy(img);
    return;
}

void UsbCamera::getVideoProperties()
{
    qDebug() << "Width of the frames in the video stream: " << m_capture->get(CV_CAP_PROP_FRAME_WIDTH) << endl
             << "Height of the frames in the video stream: " << m_capture->get(CV_CAP_PROP_FRAME_HEIGHT) << endl
             << "Frame rate: " << m_capture->get(CV_CAP_PROP_FPS) << endl
             << "4-character code of codec: " << m_capture->get(CV_CAP_PROP_FOURCC) << endl
             << "Format of the Mat objects returned by retrieve(): " << m_capture->get(CV_CAP_PROP_FORMAT) << endl
             << "Backend-specific value indicating the current capture mode: " << m_capture->get(CV_CAP_PROP_MODE) << endl
             << "Brightness of the image: " << m_capture->get(CV_CAP_PROP_BRIGHTNESS) << endl
             << "Contrast of the image: " << m_capture->get(CV_CAP_PROP_CONTRAST) << endl
             << "Saturation of the image: " << m_capture->get(CV_CAP_PROP_SATURATION) << endl
             << "Hue of the image: " << m_capture->get(CV_CAP_PROP_HUE) << endl
             << "Gain of the image: " << m_capture->get(CV_CAP_PROP_GAIN) << endl
             << "Exposure: " << m_capture->get(CV_CAP_PROP_EXPOSURE) << endl
             << "Boolean flags indicating whether images should be converted to RGB: " << m_capture->get(CV_CAP_PROP_CONVERT_RGB) << endl
             << "The U value of the whitebalance setting: " << m_capture->get(CV_CAP_PROP_WHITE_BALANCE_U) << endl
             << "The V value of the whitebalance setting: " << m_capture->get(CV_CAP_PROP_WHITE_BALANCE_V) << endl
             << "Rectification flag for stereo cameras: " << m_capture->get(CV_CAP_PROP_RECTIFICATION) << endl
             << "The ISO speed of the camera: " << m_capture->get(CV_CAP_PROP_ISO_SPEED) << endl
             << "Amount of frames stored in internal buffer memory: " << m_capture->get(CV_CAP_PROP_BUFFERSIZE) << endl;
}
