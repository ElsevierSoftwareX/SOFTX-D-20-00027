import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {
    /* This is the element for showing the tool bar of the tracking
       view. The tool buttons can be used to expand or collapse the
       sidebar, navigate through the frames and change the view. */

    ToolBar {
        width: window.width

        RowLayout {
            anchors.fill: parent

            ToolButton {
                /* Loads the selected view. */
                id: menuButton
                height: parent.height
                width: height
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }

                Image {
                    source: "///icons/sendtoback.png"
                    fillMode: Image.PreserveAspectFit
                    anchors.fill: parent
                    anchors.margins: parent.height * 0.1
                    anchors.rightMargin: 15
                }

                menu: Menu {
                    title: "File"

                    MenuItem {
                        text: "TrackingView"
                        checkable: true
                        checked: true
                        onTriggered: mainItem.state = "TrackingView"
                    }

                    MenuItem {
                        text: "ProjectDetails"
                        checkable: true
                        onTriggered: mainItem.state = "ProjectDetails"
                    }

                    MenuItem {
                        text: "TestView"
                        checkable: true
                        onTriggered: mainItem.state = "TestView"
                    }
                }
            }

            Item {
                id: item
                width: parent.width * 0.4
            }

            ListView {
                id: listView
                width: count > 0 ? contentWidth : 0
                orientation: Qt.Horizontal
                delegate: buttonDelegate
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: item.right
                }

                model: ListModel {
                    id: listModel

                    ListElement {
                        index: 0
                        source: "///qml/views/tracking/icons/navi_arrow_rew-2.png"
                    }

                    ListElement {
                        index: 1
                        source: "///qml/views/tracking/icons/navi_arrow_play-1.png"
                    }

                    ListElement {
                        index: 2
                        source: "///qml/views/tracking/icons/navi_arrow_play-2.png"
                    }
                }
            }

            Component {
                id: buttonDelegate

                ToolButton {
                    /* Navigates through the frames. */
                    height: listView.height
                    width: height * 0.8
                    onClicked: {
                        switch(model.index) {
                            case 0:
                                break
                            case 1:
                                if(model.source === "///qml/views/tracking/icons/navi_arrow_play-1.png")
                                    listModel.setProperty(model.index, "source", "///qml/views/tracking/icons/navi_arrow_pause.png")
                                else
                                    listModel.setProperty(model.index, "source", "///qml/views/tracking/icons/navi_arrow_play-1.png")
                                break
                            case 2:
                                break
                            default:
                                break
                        }
                    }

                    Image {
                        source: model.source
                        fillMode: Image.PreserveAspectFit
                        anchors.fill: parent
                        anchors.margins: parent.height * 0.1
                    }
                }
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
                onClicked: {
                    if(sidebarIsExpanded)
                        sidebarIsExpanded = false;
                    else
                        sidebarIsExpanded = true;
                }

                Image {
                    source: sidebarIsExpanded ? "///icons/list-remove.png" : "///icons/list-add.png"
                    anchors.fill: parent
                    anchors.margins: parent.height * 0.1
                }
            }
        }
    }
}
