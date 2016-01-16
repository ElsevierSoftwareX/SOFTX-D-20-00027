import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import imb.celltracker 1.0
import "."

Item {
    function viewActivationHook() {
        objectAnnotationView.updateDisplay()
        trackAnnotationView.updateDisplay()
    }

    RowLayout {
        anchors.fill: parent

        ColumnLayout {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: sidebar.left

            Text { text: "Object Annotations" }

            CTAnnotationDisplay {
                id: objectAnnotationView
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: parent.height/2

                titleText: "Object Annotations"
                type: Annotation.OBJECT_ANNOTATION
            }


            CTAnnotationDisplay {
                id: trackAnnotationView
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                height: parent.height/2

                titleText: "Tracklet Annotations"
                type: Annotation.TRACKLET_ANNOTATION
            }
        }

        Rectangle {
            id: sidebar
            width: sidebarIsExpanded ? parent.width / 3 : 0
            color: Qt.rgba(0, 0, 0, 0.2)
            visible: sidebarIsExpanded ? true : false
            anchors {
                top: parent.top
                bottom: parent.bottom
                right: parent.right
            }

            GroupBox {
                anchors.margins: 20
                anchors.fill: parent

                Column {
                    spacing: 5
                    anchors.fill: parent

                    Button {
                        text: "Annotations"
                        anchors.left: parent.left
                        anchors.right: parent.right
                    }

                    Button {
                        text: "Dummy"
                        anchors.left: parent.left
                        anchors.right: parent.right
                    }
                }
            }
        }
    }
}
