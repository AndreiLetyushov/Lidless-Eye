#include "sgcameraimageview.h"

SGCameraImageView::SGCameraImageView(QQuickItem *parent) :
    QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
}

SGCameraImageView::~SGCameraImageView()
{
}

QSGNode *SGCameraImageView::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    node = static_cast<ImageNode *>(oldNode);

    if (!node) {

        node = new ImageNode(window());

        connect(node, SIGNAL(pendingNewTexture()), window(), SLOT(update()), Qt::QueuedConnection);
        connect(window(), SIGNAL(beforeRendering()), node, SLOT(prepareNode()), Qt::DirectConnection);
        connect(&Worker::instance(), SIGNAL(newImageIsReagy(QImage)), node, SLOT(setNewImage(QImage)), Qt::DirectConnection);

    }

    node->setRect(boundingRect());

    return node;
}






