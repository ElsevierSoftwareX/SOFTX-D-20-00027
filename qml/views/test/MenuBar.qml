import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

MenuBar {

    Menu {
        title: "File"

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

        MenuItem {
            text: "bla"
        }
    }

    Menu {
        title: "Window"

        MenuItem {
            text: "bla"
        }
    }
}
