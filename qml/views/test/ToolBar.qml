import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {

    ToolBar {
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
                        checked: true
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
                    source: "///qml/views/test/icons/CT-Logo.png"
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
                    source: sidebarIsExpanded ? "///icons/list-remove.png" : "///icons/list-add.png"
                    fillMode: Image.PreserveAspectFit
                    anchors.fill: parent
                    anchors.margins: parent.height * 0.1
                }
            }
        }
    }
}
