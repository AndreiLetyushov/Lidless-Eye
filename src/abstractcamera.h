#ifndef ABSTRACTCAMERA_H
#define ABSTRACTCAMERA_H

#include <QObject>

class AbstractCamera : public QObject
{
    Q_OBJECT
public:
    explicit AbstractCamera(QObject *parent = 0);
    virtual ~AbstractCamera();

public slots:

    virtual void initCamera(int device = 0) = 0;
    virtual void requestNewImage() = 0;

signals:
    void initCameraFinished(int result);
    void newImageIsReagy(const QImage &img);
};

#endif // ABSTRACTCAMERA_H


