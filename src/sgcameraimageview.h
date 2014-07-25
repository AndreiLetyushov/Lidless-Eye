#ifndef SGCAMERAIMAGEVIEW_H
#define SGCAMERAIMAGEVIEW_H

#include "worker.h"
#include "imagenode.h"

#include <QQuickItem>
#include <QMutex>

class SGCameraImageView : public QQuickItem
{
    Q_OBJECT

public:
    SGCameraImageView(QQuickItem *parent = 0);
    ~SGCameraImageView();

protected:
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);

private:
    ImageNode *node;
};

#endif // SGCAMERAIMAGEVIEW_H
