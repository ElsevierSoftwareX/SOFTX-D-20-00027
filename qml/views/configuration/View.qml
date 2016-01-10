import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import imb.celltracker 1.0
import "."

Item {
    function viewActivationHook() {
        /* does nothing */
    }
    ColumnLayout {
        anchors.fill: parent

        Text {
            id: cfgtxt
            text: "Configuration"
            Layout.fillWidth: parent
        }

        ListModel {
            id: lm
            ListElement { name: "name1"; desc: "description1"; value: "value1" }
            ListElement { name: "name2"; desc: "description2"; value: "value2" }
            ListElement { name: "name3"; desc: "description3"; value: "value3" }
            ListElement { name: "name4"; desc: "description4"; value: "value4" }
            ListElement { name: "name5"; desc: "description5"; value: "value5" }
        }

        Rectangle {
            id: viewArea
            border.color: "orange"
            Layout.fillHeight: parent
            Layout.fillWidth: parent
            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                ListView {
                    id: tv
                    model: lm
                    anchors.fill: parent


                    delegate: Rectangle {
                        height: 30
                        width: parent.width
//                        border.color: "red"
                        RowLayout {
                            anchors.fill: parent
                            spacing: 0
                            Rectangle {
//                                border.color: "green"
                                Layout.fillHeight: parent
                                width: 300
                                ColumnLayout {
                                    spacing: 0
                                    anchors.fill: parent

                                    Text { text: model.name }
                                    Text { text: model.desc; color: "gray" }
                                }
                            }
                            Rectangle {
                                Layout.fillWidth: parent
                                Layout.fillHeight: parent
                                TextField {
                                    anchors.verticalCenter: parent.verticalCenter
                                    Layout.fillWidth: parent
                                    text: model.value
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
