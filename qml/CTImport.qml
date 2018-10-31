/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2016 Konstantin Thierbach, Sebastian Wagner
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
import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import imb.celltracker 1.0

Window {
    id: importWin
    title: "Import"
    modality: Qt.WindowModal

    height: 500
    width: 500
    flags: Qt.Dialog

    Loader {
        id: pageLoader
        sourceComponent: typeSelection
        anchors.fill: parent
    }

    Component {
        id: typeSelection

        Rectangle {
            anchors.fill: parent

            Rectangle { /* needed for centering vertically */
                width: parent.width
                height: lv.contentHeight
                anchors.centerIn: parent

                property list<QtObject> typeModel: [
                    QtObject { property string name: "Fiji XML";           property QtObject component: fijiSettings },
                    QtObject { property string name: "Fluid Tracking XML"; property QtObject component: fluidSettings },
                    QtObject { property string name: "Cell Profiler";      property QtObject component: profilerSettings }
                ]

                ListView {
                    id: lv
                    anchors.fill: parent
                    anchors.margins: 5
                    model: parent.typeModel
                    spacing: 5

                    delegate: Button {
                        text: model.name

                        anchors.horizontalCenter: parent.horizontalCenter
                        height: 30
                        width: 200

                        onClicked: pageLoader.sourceComponent = model.component
                    }
                }
            }
        }
    }

    Component {
        id: fijiSettings

        Rectangle {
            id: fijiMain
            anchors.fill: parent

            border.color: "red"
            border.width: 3

            function doImport() {
                var out = [];
                out["projectFile"] = _projectFile
                out["rows"] = _rows
                out["cols"] = _cols
                out["slices"] = []

                for (var i = 0; i < _slice_count; i++) {
                    var slice = rep.itemAt(i)
                    out["slices"][i] = []
                    out["slices"][i]["tracks"] = slice._tracks
                    out["slices"][i]["xml"] = slice._xml
                    out["slices"][i]["channels"] = []

                    for (var j = 0; j < slice._channel_count; j++) {
                        var channel = slice._channels.itemAt(j)
                        out["slices"][i]["channels"][j] = channel._images
                    }
                }

                DataProvider.importFiji(out)
                GUIState.projPath = _projectFile;
                statusWindow.show()
                importWin.close()
            }

            function printArray(arr) {
                var ret = "";
                if (arr instanceof Array) {
                    ret += "["
                    for (var key in arr) {
                        ret += key + ": " + printArray(arr[key]) + ", "
                    }
                    ret += "]"
                } else {
                    ret += arr
                }
                return ret
            }

            property string _projectFile: projFile.text
            property int _rows: layoutRows.text*1
            property int _cols: layoutCols.text*1
            property int _slice_count: rep.count
            property var _slices: rep

            FileDialog {
                property TextField target
                property string lastFolder: "."

                id: fd
                modality: Qt.WindowModal
                visible: false
                nameFilters: "*"
                selectExisting: true
                selectFolder: false
                selectMultiple: false

                function get(tar, filter, exist, folder, multi) {
                    fd.target = tar
                    fd.folder = lastFolder
                    fd.nameFilters = filter
                    fd.selectExisting = exist
                    fd.selectFolder = folder
                    fd.selectMultiple = multi
                    fd.visible = true
                }

                onAccepted: {
                    target.text = DataProvider.localFileFromURL(fileUrl)
                    lastFolder = folder
                }
            }

            ScrollView {
                anchors.fill: parent


                ColumnLayout {
                    id: gl
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    Row {
                        height: 30
                        width: 500
                        Text {
                            height: 30;
                            width: parent.width
                            font.pixelSize: 20
                            text: "Fiji XML"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                    Row {
                        height: 30
                        Text {
                            height: 30
                            width: 200
                            text: "Project File: "
                            verticalAlignment: Text.AlignVCenter
                        }
                        TextField {
                            id: projFile
                            height: 30
                            width: 200
                            text: "proj.h5"
                        }
                        Button {
                            text: "..."
                            onClicked: fd.get(projFile, "*.h5", false, false, false)
                        }
                    }
                    Row {
                        height: 30
                        Text {
                            height: 30
                            width: 200
                            text: "Slice Layout: "
                            verticalAlignment: Text.AlignVCenter
                        }
                        TextField {
                            id: layoutRows
                            height: 30
                            width: 50
                            text: "rows"
                            horizontalAlignment: Text.AlignHCenter
                        }
                        Text {
                            height: 30
                            width: 30
                            text: "x"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        TextField {
                            id: layoutCols
                            height: 30
                            width: 50
                            text: "cols"
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }

                    Repeater {
                        id: rep
                        model: 1

                        delegate: Column {
                            property int _index: index
                            property string _tracks: tracks.text
                            property string _xml: xml.text
                            property int _channel_count: crep.count
                            property var _channels: crep

                            Row {
                                Text {
                                    height: 30
                                    width: 500
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                    font.pixelSize: 16
                                    text: "Slice " + index
                                }
                            }
                            Row {
                                height: 30
                                Text {
                                    height: 30
                                    width: 200
                                    verticalAlignment: Text.AlignVCenter
                                    text: "Tracklet File"
                                }
                                TextField {
                                    id: tracks
                                    height: 30
                                    width: 200
                                    text: "tracksXML.xml"
                                }
                                Button {
                                    text: "..."
                                    onClicked: fd.get(tracks, "*.xml", true, false, false)
                                }
                            }
                            Row {
                                height: 30
                                Text {
                                    height: 30
                                    width: 200
                                    verticalAlignment: Text.AlignVCenter
                                    text: "XML Folder"
                                }
                                TextField {
                                    id: xml
                                    height: 30
                                    width: 200
                                    text: "xml"
                                }
                                Button {
                                    text: "..."
                                    onClicked: fd.get(xml, "*", true, true, false)
                                }
                            }
                            Repeater {
                                id: crep
                                model: 1
                                delegate: Row {
                                    property int _index: index
                                    property string _images: images.text
                                    Text {
                                        height: 30
                                        width: 200
                                        verticalAlignment: Text.AlignVCenter
                                        text: "Channel " + index + " images: "
                                    }
                                    TextField {
                                        id: images
                                        height: 30
                                        width: 200
                                        text: "images"
                                    }
                                    Button {
                                        text: "..."
                                        onClicked: fd.get(images, "*", true, true, false)
                                    }
                                }
                            }
                            Row {
                                height: 30
                                Button { height: 30; width: 150; text: "Add Channel"; onClicked: crep.model++ }
                                Button { height: 30; width: 150; text: "Remove Channel"; onClicked: crep.model-- }
                            }
                        }
                    }
                    Row {
                        height: 30
                        Button { height: 30; width: 150; text: "Add Slice"; onClicked: rep.model++ }
                        Button { height: 30; width: 150; text: "Remove Slice"; onClicked: rep.model-- }
                    }
                    Rectangle {
                        Layout.fillHeight: true
                        Row {
                            height: 30
                            width: 500
                            Layout.fillWidth: true
                            layoutDirection: Qt.RightToLeft
                            Button { height: 30; text: "Import"; onClicked: fijiMain.doImport() }
                            Button { height: 30; text: "Back"; onClicked: pageLoader.sourceComponent = typeSelection }
                        }
                    }
                }
            }
        }
    }

    Component {
        id: fluidSettings

        Rectangle {
            anchors.fill: parent

            ColumnLayout {
                anchors.fill: parent

                Text { text: "Fluid tracking xml is unimplemented" }
            }
        }
    }

    Component {
        id: profilerSettings

        Rectangle {
            anchors.fill: parent

            ColumnLayout {
                anchors.fill: parent

                Text { text: "Cell profiler is unimplemented" }
            }
        }
    }

    function open() { importWin.visible = true }
    function close() { importWin.visible = false }
}
