import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {

    ToolBar {
        height: window.height / 10
        width: window.width

        RowLayout {
            anchors.fill: parent

            ToolButton {
                id: firstButton
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

            ToolButton {
                id: secondButton
                height: parent.height
                width: height
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: firstButton.right
                }

                Image {
                    source: "///img/CT-Logo.png"
                    fillMode: Image.PreserveAspectFit
                    anchors.fill: parent
                    anchors.margins: parent.height * 0.1
                }
            }

            Item {
                Layout.fillWidth: true
            }

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
                        sidebarIsExpanded = false
                    else
                        sidebarIsExpanded = true
                }

                Image {
                    source: sidebarIsExpanded ? "///img/list-remove.png" : "///img/list-add.png"
                    fillMode: Image.PreserveAspectFit
                    anchors.fill: parent
                    anchors.margins: parent.height * 0.1
                }
            }
        }
    }
}
