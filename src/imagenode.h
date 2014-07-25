#ifndef IMAGENODE_H
#define IMAGENODE_H

#include <QObject>
#include <QQuickItem>
#include <QSGSimpleTextureNode>
#include <QSGTexture>
#include <QQuickWindow>
#include <QMutex>

#include <memory>


class ImageNode : public QObject, public QSGSimpleTextureNode
{
    Q_OBJECT
public:
    ImageNode(QQuickWindow *window);
    ~ImageNode();

signals:
    void textureInUse();
    void pendingNewTexture();

public slots:
    // Before the scene graph starts to render, we update to the pending texture
    void prepareNode();
    void setNewImage(const QImage &newImage);

private:

    QImage m_imgFromCam;
    QMutex m_mutex;

    QSGTexture *m_texture;
    QQuickWindow *m_window;
};



#endif // IMAGENODE_H
