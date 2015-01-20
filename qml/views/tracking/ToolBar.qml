import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {

    ToolBar {
        id: toolBar
        height: window.height / 10
        width: window.width

        RowLayout {
            anchors.fill: parent

            ToolButton {
                id: menuButton
                height: parent.height
                width: height
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }

                Image {
                    source: "///img/sendtoback.png"
                    fillMode: Image.PreserveAspectFit
                    anchors.fill: parent
                    anchors.margins: parent.height * 0.1
                    anchors.rightMargin: 20
                }

                menu: Menu {
                    title: "File"

                    MenuItem {
                        text: "TrackingView"
                        onTriggered: mainItem.state = "TrackingView"
                    }

                    MenuItem {
                        text: "ProjectDetails"
                        onTriggered: mainItem.state = "ProjectDetails"
                    }

                    MenuItem {
                        text: "TestView"
                        onTriggered: mainItem.state = "TestView"
                    }
                }
            }

            /*ToolButton {
                id: generatedStartButton
                text: "generated\ntrack start"
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: menuButton.right
                }
            }

            ToolButton {
                id: startButton
                text: "track\nstart"
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: generatedStartButton.right
                }
            }*/

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

                    /*ListElement {
                        index: 1
                        source: "///img/navi_arrow_rew-3.png"
                    }*/

                    ListElement {
                        index: 0
                        source: "///img/navi_arrow_rew-2.png"
                    }

                    /*ListElement {
                        index: 3
                        source: "///img/navi_arrow_rew-1.png"
                    }

                    ListElement {
                        index: 4
                        source: "///img/navi_arrow_pause.png"
                    }*/

                    ListElement {
                        index: 1
                        source: "///img/navi_arrow_play-1.png"
                    }

                    ListElement {
                        index: 2
                        source: "///img/navi_arrow_play-2.png"
                    }

                    /*ListElement {
                        index: 7
                        source: "///img/navi_arrow_play-3.png"
                    }*/
                }
            }

            Component {
                id: buttonDelegate

                ToolButton {
                    height: listView.height
                    width: height * 0.8
                    onClicked: {
                        switch(model.index) {
                            case 0:
                                break
                            case 1:
                                if(model.source === "///img/navi_arrow_play-1.png")
                                    listModel.setProperty(model.index, "source", "///img/navi_arrow_pause.png")
                                else
                                    listModel.setProperty(model.index, "source", "///img/navi_arrow_play-1.png")
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

            /*ToolButton {
                id: endButton
                text: "track\nend"
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: listView.right
                }
            }

            ToolButton {
                text: "generated\ntrack end"
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: endButton.right
                }
            }*/

            ToolButton {
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
                    source: sidebarIsExpanded ? "///img/list-remove.png" : "///img/list-add.png"
                    anchors.fill: parent
                    anchors.margins: parent.height * 0.1
                }
            }
        }
    }
}
