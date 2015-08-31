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
        onHeightChanged: parent.height = height
        onWidthChanged: parent.width = width

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
                onClicked: {
                    if(sidebarIsExpanded)
                        sidebarIsExpanded = false;
                    else
                        sidebarIsExpanded = true;
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
