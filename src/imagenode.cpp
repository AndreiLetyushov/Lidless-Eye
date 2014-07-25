#include "imagenode.h"

ImageNode::ImageNode(QQuickWindow *window)
    : m_imgFromCam(0), m_texture(0), m_window(window)
{
    // Our texture node must have a texture, so use the default 0 texture.
    m_texture = m_window->createTextureFromId(0, QSize(1, 1));
    setTexture(m_texture);
    setFiltering(QSGTexture::Linear);
}

ImageNode::~ImageNode()
{
    delete m_texture;
}

void ImageNode::prepareNode() {
    m_mutex.lock();
    QImage newImg(m_imgFromCam);
    m_imgFromCam = QImage();
    m_mutex.unlock();

    if (!newImg.isNull()) {
        delete m_texture;
        m_texture = m_window->createTextureFromImage(newImg);
        setTexture(m_texture);

        // This will notify the rendering thread that the texture is now being rendered
        // and it can start rendering to the other one.
        emit textureInUse();
    }
}

void ImageNode::setNewImage(const QImage &newImage)
{
    m_mutex.lock();
    m_imgFromCam = newImage.copy();
    m_mutex.unlock();

    emit pendingNewTexture();
}
