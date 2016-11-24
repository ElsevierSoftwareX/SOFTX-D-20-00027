import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1
import imb.celltracker 1.0
import "."

Item {
    function viewActivationHook() {
        objectAnnotationView.updateDisplay()
        trackAnnotationView.updateDisplay()
        trackletView.updateDisplay()
    }

    function viewDeactivationHook() { }

    RowLayout {
        anchors.fill: parent

        Item {
            id: contentPane
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: sidebar.left

            children: [tracklets]

            ColumnLayout {
                id: annotations
                anchors.fill: parent

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

            ColumnLayout {
                id: tracklets
                anchors.fill: parent

                CTTrackletDisplay {
                    id: trackletView
                    anchors.fill: parent

                    titleText: "Tracklets"
                }
            }

            ColumnLayout {
                id: time
                anchors.fill: parent

                property int workSeconds : GUIState.workedOnProject
                property int displaySeconds : workSeconds % 60       // Seconds without Minutes
                property int displayMinutes : workSeconds/60 % 3600  // Minutes without Hours
                property int displayHours : workSeconds/3600 % 86400 // Hours without Days
                property int displayDays : workSeconds/86400         // Days

                Text { text: "Worked on project: "
                             + parent.displayDays.toLocaleString("%02.0f") + "d "
                             + parent.displayHours + "h "
                             + parent.displayMinutes + "m "
                             + parent.displaySeconds + "s" }
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
                        onClicked: contentPane.children = [annotations]
                    }

                    Button {
                        text: "Tracklets"
                        anchors.left: parent.left
                        anchors.right: parent.right
                        onClicked: contentPane.children = [tracklets]
                    }

                    Button {
                        text: "Time"
                        anchors.left: parent.left
                        anchors.right: parent.right
                        onClicked: contentPane.children = [time]
                    }
                }
            }
        }
    }
}
