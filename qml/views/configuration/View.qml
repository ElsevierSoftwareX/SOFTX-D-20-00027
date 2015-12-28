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

        Text { id: cfgtxt; text: "Configuration" }

        ListModel {
            id: lm
            ListElement { name: "name1"; desc: "description1"; value: "value1" }
            ListElement { name: "name2"; desc: "description2"; value: "value2" }
            ListElement { name: "name3"; desc: "description3"; value: "value3" }
        }

        ListView {
            id: lv
            model: lm

//            Layout.fillHeight: parent
            anchors.top: cfgtxt.bottom
            anchors.bottom: parent.bottom
//            height: parent.height - cfgtxt.height
            width: parent.width
//            Layout.fillWidth: parent

            delegate: RowLayout {
                width: lv.width
                height: 50

                Rectangle {
                    border.color: "green"
                    height: 10
                    width: lv.width*(1/3)
//                    width: 100
//                    width: lv.width*(1/3)
//                    Text {
//                        id: tn
//                        height: tn.contentHeight
//                        width: parent.width
//                        horizontalAlignment: Text.AlignRight
//                        verticalAlignment: Text.AlignHCenter
//                        text: "A"
////                        text: model.name + " " + model.desc
//                    }
                }
                Rectangle {
                    height: 10
//                    Layout.fillWidth: parent
//                    width: 100
                    width: lv.width*(2/3)
                    border.color: "red"
//                    Text {
//                        id: tv
//                        height: tv.contentHeight
//                        width: parent.width
//                        horizontalAlignment: Text.AlignLeft
//                        verticalAlignment: Text.AlignHCenter
//                        text: "B"
////                        text: model.value
//                    }
                }
            }
        }
    }
}
