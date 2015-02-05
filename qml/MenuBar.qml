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

            // Auswahl Importformat XML,HDF5, Button NEXT
            // bei XML: Projectname: Eingabefeld
            // Projectfolder: Auswahlmenü (FileOpnDialog nur Ordner), OK; CANCEL
            // bei HDF5: Projectfile: Eingabe (SELECT) OK; CANCEL
        }

        MenuItem {
            text: "Open Project"
            shortcut: StandardKey.Open
            onTriggered: fileDialog.visible = true

            // Datei öffnen!! FileDialog HDF5-Datei auswählen
        }

        MenuSeparator {}

        MenuItem {
            text: "Save Current Project"
            shortcut: StandardKey.Save
        }

        MenuItem {
            text: "Save Current Project As"
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
        title: "Project"
        enabled: trackingViewIsVisible
        visible: trackingViewIsVisible

        MenuItem {
            text: "Project Details"
        }

        MenuItem {
            text: "Offset/Point Of Origin"
        }
    }

    Menu {
        title: "Working View"
        enabled: trackingViewIsVisible
        visible: trackingViewIsVisible

        MenuItem {
            text: "Show Cell Items"
        }

        MenuItem {
            text: "Show Only Images"
            shortcut: StandardKey.Italic
        }

        MenuSeparator {}

        MenuItem {
            text: "Base Size"
            shortcut: StandardKey.Bold
        }

        MenuItem {
            text: "Zoom In"
            shortcut: StandardKey.ZoomIn
        }

        MenuItem {
            text: "Zoom Out"
            shortcut: StandardKey.ZoomOut
        }

        MenuSeparator {}

        MenuItem {
            text: "Scroll Hand Drag"
        }

        MenuItem {
            text: "Scroll Bar As Needed"
        }

        MenuSeparator {}

        MenuItem {
            text: "Show Working View Info"
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
