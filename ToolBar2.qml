import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Component {

    ToolBar {
        height: window.height / 10
        width: window.width

        RowLayout {
        anchors.fill: parent

            ToolButton {
                id: firstButton
                height: parent.height
                width: 1.5 * height
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }

                Image {
                    source: "img/navi_arrow_play-1.png"
                    anchors.fill: parent
                    anchors.margins: parent.height * 0.1
                    anchors.rightMargin: 10
                }

                menu: Menu {
                    title: "File"

                    MenuItem {
                        text: "View 1"
                        onTriggered: currentView = 1
                    }

                    MenuItem {
                        text: "View 2"
                        onTriggered: currentView = 2
                    }
                }
            }

            ToolButton {
                id: secondButton
                height: parent.height
                width: 1.5 * height
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: firstButton.right
                }

                Image {
                    source: "img/CT-Logo.png"
                    anchors.fill: parent
                    anchors.margins: parent.height * 0.1
                }
            }

            Item {
                Layout.fillWidth: true
            }

            ToolButton {
                height: parent.height
                width: 1.5 * height
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
                    source: "img/navi_arrow_pause.png"
                    anchors.fill: parent
                    anchors.margins: parent.height * 0.1
                }
            }
        }
    }
}
