import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {

    RowLayout {

        Label {
            id: label
            text: mousePosition.status

            Connections {
                target: myImport
                onEventTriggered: label.text = status
            }
        }
    }
}
