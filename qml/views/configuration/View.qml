/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2016, 2015 Sebastian Wagner
 *
 * TraCurate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TraCurate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with TraCurate.  If not, see <https://www.gnu.org/licenses/>.
 */
import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import imb.tracurate 1.0
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
            Layout.fillWidth: true
        }

        ListModel {
            id: lm
        }

        function buildModel() {
            lm.clear()
            var c = TCSettings.getConfiguration()
            for (var i = 0; i < c.length; i++) {
                lm.append({ "name"       : c[i].name,
                            "modifiable" : c[i].modifiable,
                            "cName"      : c[i].cName,
                            "desc"       : c[i].desc,
                            "type"       : c[i].type,
                              // convert to string…
                            "value"      : "" + TCSettings.value(c[i].name) });
            }
        }

        Component.onCompleted: buildModel()

        Rectangle {
            id: viewArea
            border.color: "orange"
            Layout.fillHeight: true
            Layout.fillWidth: true
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
                                Layout.fillHeight: true
                                width: 500
                                ColumnLayout {
                                    spacing: 0
                                    anchors.fill: parent

                                    Text { text: model.cName }
                                    Text { text: model.desc; color: "gray" }
                                }
                            }
                            Rectangle {
                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                Loader {
                                    sourceComponent: {
                                        switch (model.type) {
                                        case "number": return numberDelegate;
                                        case "percent": return percentDelegate;
                                        case "color": return colorDelegate;
                                        case "bool": return boolDelegate;
                                        }
                                    }
                                }

                                Component {
                                    id: boolDelegate
                                    CheckBox {
                                        id: cB
                                        anchors.centerIn: parent
                                        partiallyCheckedEnabled: false

                                        enabled: model.modifiable
                                        checked: model.value === "true"

                                        onCheckedChanged: TCSettings.setValue(model.name, cB.checked)
                                    }
                                }

                                Component {
                                    id: numberDelegate
                                    TextField {
                                        id: tf
                                        implicitWidth: 100
                                        anchors.verticalCenter: parent.verticalCenter

                                        enabled: model.modifiable
                                        text: model.value

                                        onAccepted: TCSettings.setValue(model.name, tf.text*1)
                                    }
                                }

                                Component {
                                    id: percentDelegate
                                    Slider {
                                        id: sldr
                                        implicitWidth: 100

                                        updateValueWhileDragging: false
                                        minimumValue: 0
                                        maximumValue: 100
                                        value: model.value*100

                                        style: SliderStyle { /* Basically the verbatim SliderStyle from Qt, but without the FastGlow (which causes problems on Windows) */
                                            handle: Item {
                                                implicitWidth:  implicitHeight
                                                implicitHeight: TextSingleton.implicitHeight * 1.2

                                                Rectangle {
                                                    id: handle
                                                    anchors.fill: parent
                                                    radius: width/2
                                                    gradient: Gradient { GradientStop { color: control.pressed ? "#e0e0e0" : "#fff"; position: 1 }
                                                                         GradientStop { color: "#eee"; position: 0 } }
                                                    Rectangle {
                                                        anchors.fill: parent
                                                        anchors.margins: 1
                                                        radius: width/2
                                                        border.color: "#99ffffff"
                                                        color: control.activeFocus ? "#224f7fbf" : "transparent"
                                                    }
                                                    border.color: control.activeFocus ? "#47b" : "#777"
                                                }
                                            }
                                        }

                                        onValueChanged: TCSettings.setValue(model.name, sldr.value/100)
                                    }
                                }

                                Component {
                                    id: colorDelegate
                                    Button {
                                        id: btn
                                        property string colValue: model.value

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
                                                TCSettings.setValue(model.name, cd.color)
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
}
