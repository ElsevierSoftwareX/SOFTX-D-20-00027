import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.3
import QtQuick.Layouts 1.1
import imb.celltracker 1.0
import "."

Item {
    function viewActivationHook() { cl.buildModel() }
    function viewDeactivationHook() { }

    ColumnLayout {
        anchors.fill: parent
        id: cl

        Text {
            id: cfgtxt
            text: "Configuration"
            Layout.fillWidth: parent
        }

        ListModel {
            id: lm
        }

        function buildModel() {
            lm.clear()
            var c = CTSettings.getConfiguration()
            for (var i = 0; i < c.length; i++) {
                lm.append({ "name"       : c[i].name,
                            "modifiable" : c[i].modifiable,
                            "cName"      : c[i].cName,
                            "desc"       : c[i].desc,
                            "type"       : c[i].type,
                              // convert to string…
                            "value"      : "" + CTSettings.value(c[i].name) });
                console.log("while rebuilding: " + c[i].name + " = " + CTSettings.value(c[i].name))
            }
        }

        Component.onCompleted: buildModel()

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
                                    visible: model.type === "number"
                                    implicitWidth: 100
                                    anchors.verticalCenter: parent.verticalCenter
                                    enabled: model.modifiable
                                    text: model.value

                                    onAccepted: enabled && CTSettings.setValue(model.name, tf.text*1)
                                }
                                Slider {
                                    id: sldr
                                    visible: model.type === "percent"
                                    enabled: model.type === "percent"
                                    implicitWidth: 100
                                    updateValueWhileDragging: false

                                    minimumValue: 0
                                    maximumValue: 100
                                    value: model.value*100

                                    onValueChanged: enabled && CTSettings.setValue(model.name, sldr.value/100)
                                }
                                Button {
                                    property string colValue: model.value

                                    id: btn
                                    visible: model.type === "color"

                                    implicitWidth: 100
                                    anchors.verticalCenter: parent.verticalCenter
                                    style: ButtonStyle {
                                        id: stl
                                        background: Rectangle {
                                            id: btnBg
                                            radius: 4
                                            color: btn.colValue
                                            border.width: 1
                                            border.color: "gray"
                                        }
                                        label: Text {
                                            text: btn.colValue
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                            property color base: btn.colValue
                                            color: Qt.rgba((1-base.r), (1-base.g), (1-base.b), 1)
                                        }
                                    }
                                    onClicked: cd.open()

                                    ColorDialog {
                                        id: cd
                                        onAccepted: {
                                            btn.colValue = cd.color
                                            CTSettings.setValue(model.name, cd.color)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
