#ifndef CAMERAIMAGEVIEW_H
#define CAMERAIMAGEVIEW_H

#include "worker.h"

#include <QQuickPaintedItem>
#include <QImage>
#include <QMutex>

class QPainter;

class CameraImageView : public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit CameraImageView(QQuickItem *parent = 0);

public slots:
    void setNewImage(const QImage &newImage);

private:
    void paint(QPainter *painter);

    QImage m_imgFromCam;
    QMutex m_mutex;
};

#endif // CAMERAIMAGEVIEW_H
