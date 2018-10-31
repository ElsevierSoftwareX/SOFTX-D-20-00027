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
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1
import imb.celltracker 1.0

Item {
    /* This is the element for showing the tool bar of the tracking
       view. The tool buttons can be used to expand or collapse the
       sidebar, navigate through the frames and change the view. */
    width: window.width
    height: 50

    ToolBar {
        anchors.fill: parent

        RowLayout {
            anchors.fill: parent

            ExclusiveGroup { id: viewGroup }

            ListView {
                model: mainItem.stateList
                orientation: ListView.Horizontal
                spacing: 5
                anchors.fill: parent

                delegate: ToolButton {
                    /* Loads the selected view. */
                    id: viewButton
                    enabled: true
                    visible: viewButton.enabled
                    width: Math.max(nameText.width, buttonImage.width)
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    checkable: true
                    exclusiveGroup: viewGroup

                    Action {
                        shortcut: "Ctrl+"+(index+1)
                        enabled: viewButton.enabled
                        onTriggered: {
                            currentMainWindow.item.viewDeactivationHook()
                            mainItem.state = model.stateName
                        }
                    }

                    ColumnLayout {
                        id: cl
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom

                        Layout.alignment: Qt.AlignHCenter
                        Image {
                            id: buttonImage
                            source: model.iconPath
                            fillMode: Image.PreserveAspectFit
                            anchors.top: parent.top
                            anchors.bottom: nameText.top
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        Text {
                            id: nameText
                            anchors.bottom: parent.bottom
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: model.stateName
                            font.pixelSize: CTSettings.value("text/default_fontsize")
                            width: parent.width
                        }
                    }
                    onClicked: {
                        currentMainWindow.item.viewDeactivationHook()
                        mainItem.state = model.stateName
                    }
                }
            }

            Item {
                Layout.fillWidth: true
            }

            ToolButton {
                /* Expands and collapses the sidebar. */
                height: parent.height
                width: height
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: parent.right
                }
                onClicked: sidebarIsExpanded = !sidebarIsExpanded

                Image {
                    source: sidebarIsExpanded ? "///icons/iconmonstr-minus-1.svg" : "///icons/iconmonstr-plus-1.svg"
                    fillMode: Image.PreserveAspectFit
                    anchors.fill: parent
                    anchors.margins: parent.height * 0.3
                }
            }
        }
    }
}
