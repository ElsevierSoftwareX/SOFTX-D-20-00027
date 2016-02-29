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
        }

        Component.onCompleted: {
            lm.clear()
            var c = CTSettings.getConfiguration()
            for (var i = 0; i < c.length; i++) {
                lm.append({ "name" : c[i].name,
                            "modifiable" : c[i].modifiable,
                            "cName" : c[i].cName,
                            "desc" : c[i].desc,
                              // convert to string…
                            "value" : "" + CTSettings.value(c[i].name) });
            }
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
                                width: 500
                                ColumnLayout {
                                    spacing: 0
                                    anchors.fill: parent

                                    Text { text: model.cName }
                                    Text { text: model.desc; color: "gray" }
                                }
                            }
                            Rectangle {
                                Layout.fillWidth: parent
                                Layout.fillHeight: parent
                                TextField {
                                    id: tf
                                    anchors.verticalCenter: parent.verticalCenter
                                    Layout.fillWidth: parent
                                    enabled: model.modifiable
                                    text: model.value
                                    onAccepted: CTSettings.setValue(model.name, tf.text)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
