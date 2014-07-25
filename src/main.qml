import QtQuick 2.2
import recognition.components 1.0

Item {
    visible: true
    width: 800
    height: 400

    CameraImageView {
        id: cameraView
        height: parent.height
        width: parent.width/2
    }

    SGCameraImageView {
        id: cameraViewSG
        anchors.right: parent.right
        height: parent.height
        width: parent.width/2
     }
}


