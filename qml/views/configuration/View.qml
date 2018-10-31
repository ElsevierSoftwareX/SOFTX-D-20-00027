/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2016, 2015 Sebastian Wagner
 *
 * Celltracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Celltracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Celltracker.  If not, see <https://www.gnu.org/licenses/>.
 */
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
            Layout.fillWidth: true
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

                                        onCheckedChanged: CTSettings.setValue(model.name, cB.checked)
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

                                        onAccepted: CTSettings.setValue(model.name, tf.text*1)
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

                                        onValueChanged: CTSettings.setValue(model.name, sldr.value/100)
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
}
