import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

MenuBar {
    property bool trackingViewIsVisible: mainItem.state === "TrackingView" ? true : false
    property bool testViewIsVisible: mainItem.state === "TestView" ? true : false

    Menu {
        title: "File"
        enabled: trackingViewIsVisible
        visible: trackingViewIsVisible

        MenuItem {
            text: "New Project"
            shortcut: StandardKey.New
            onTriggered: dialog.visible = true
        }

        MenuItem {
            text: "Open Project"
            shortcut: StandardKey.Open
            onTriggered: fileDialog.visible = true
        }

        MenuSeparator {}

        MenuItem {
            text: "Save Project"
            shortcut: StandardKey.Save
        }

        MenuItem {
            text: "Save Project As"
        }

        MenuSeparator {}

        Menu {
            title: "Export"

            MenuItem {
                text: "Export New Tracks To XML"
            }
        }

        MenuItem {
            text: "Exit"
            onTriggered: Qt.quit()
        }
    }

    Menu {
        title: "View"
        enabled: trackingViewIsVisible
        visible: trackingViewIsVisible

        MenuItem {
            text: "Object Outlines"
            shortcut: StandardKey.Open
            checkable: true
        }

        MenuSeparator {}

        MenuItem {
            text: "Object Info"
            checkable: true
        }

        MenuItem {
            text: "Track Info"
            checkable: true
        }

        MenuItem {
            text: "Strategies"
            checkable: true
        }

        MenuItem {
            text: "Operations"
            checkable: true
        }

        MenuSeparator {}

        MenuItem {
            text: "Zoom In"
            shortcut: StandardKey.ZoomIn
        }

        MenuItem {
            text: "Zoom Out"
            shortcut: StandardKey.ZoomOut
        }
    }

    Menu {
        title: "File"
        enabled: testViewIsVisible
        visible: testViewIsVisible

        MenuItem {
            text: "bla"
        }

        MenuItem {
            text: "Exit"
            onTriggered: Qt.quit()
        }
    }

    Menu {
        title: "View"
        enabled: testViewIsVisible
        visible: testViewIsVisible

        MenuItem {
            text: "bla"
        }
    }

    Menu {
        title: "Window"
        enabled: testViewIsVisible
        visible: testViewIsVisible

        MenuItem {
            text: "bla"
        }
    }

    Menu {
        title: "Help"

        MenuItem {
            text: "Show Desktop Info"
        }

        MenuItem {
            text: "How To Do (Tutorial)"
        }
    }
}
