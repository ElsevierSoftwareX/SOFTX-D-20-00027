import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {
    RowLayout {
        Label {
            id: statusBar
            text: mousePosition.status
            Connections {
                target: messageRelay
                onUpdateStatusBar: {
                    mousePosition.status = message
                }
            }
        }
    }
}
