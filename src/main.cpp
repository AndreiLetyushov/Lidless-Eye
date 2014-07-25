#include "cameraimageview.h"
#include "sgcameraimageview.h"
#include "worker.h"

#include <QGuiApplication>
#include <QQuickView>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType<SGCameraImageView>("recognition.components", 1, 0, "SGCameraImageView");
    qmlRegisterType<CameraImageView>("recognition.components", 1, 0, "CameraImageView");

    QQuickView view;
    view.setPersistentOpenGLContext(true);
    view.setPersistentSceneGraph(true);

    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl("qrc:///main.qml"));
    view.show();

    return app.exec();
}
