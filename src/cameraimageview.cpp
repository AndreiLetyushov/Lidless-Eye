#include "cameraimageview.h"

CameraImageView::CameraImageView(QQuickItem *parent)
    : QQuickPaintedItem(parent), m_imgFromCam(0)
{
    connect(&Worker::instance(), SIGNAL(newImageIsReagy(QImage)), this, SLOT(setNewImage(QImage)), Qt::DirectConnection);
}

void CameraImageView::paint(QPainter *painter)
{
    m_mutex.lock();
    QImage newImg(m_imgFromCam);
    m_imgFromCam = QImage();
    m_mutex.unlock();

    painter->save();

    painter->setRenderHint(QPainter::Antialiasing);

    painter->drawImage(QRectF(0.0f,0.0f, this->width(), this->height()), newImg);

    painter->restore();
}



void CameraImageView::setNewImage(const QImage &newImage)
{
    m_mutex.lock();
    m_imgFromCam = newImage.copy();
    m_mutex.unlock();

    update();
}
