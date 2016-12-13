import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1
import QtQml 2.2
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

                property var wop: GUIState.workedOnProject

                onWopChanged: {
                    lm.update()
                }

                ListModel {
                    id: lm

                    function update() {
                        lm.clear()
                        for (var k in time.wop) {
                            lm.append({ "date" : k,
                                        "sum" : time.wop[k]})
                        }
                    }
                }

                ColumnLayout {
                    anchors.fill: parent

                    TableView {
                        model: lm
                        anchors.fill: parent

                        TableViewColumn {
                            role: "date"
                            title: "Date"
                            width: 200
                            delegate: Text {
                                id: date_text
                                anchors.fill: parent
                                height: 20
                                text: model.date
                            }
                        }
                        TableViewColumn {
                            role: "sum"
                            title: "Time Tracked"
                            width: 200
                            delegate:  Text {
                                property var d: new Date(Date.UTC(0, 0, 0, 0, 0, model.sum, 0)) // Yay, JavaScript
                                anchors.fill: parent
                                id: sum_text
                                text: d.getUTCHours() + "h " + d.getUTCMinutes() + "m " + d.getUTCSeconds() + "s"
                            }
                        }
                    }
                }
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
