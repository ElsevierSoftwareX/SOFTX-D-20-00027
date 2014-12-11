import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

MenuBar {

    Menu {
        title: "File"

        MenuItem {
            text: "New Project"
            shortcut: StandardKey.New
        }

        MenuItem {
            text: "Open Project"
            shortcut: StandardKey.Open
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
    }

    Menu {
        title: "Project"

        MenuItem {
            text: "Project Details"
        }

        MenuItem {
            text: "Offset/Point Of Origin"
        }
    }

    Menu {
        title: "Working View"

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
        title: "Help"

        MenuItem {
            text: "Show Desktop Info"
        }

        MenuItem {
            text: "How To Do (Tutorial)"
        }
    }
}
