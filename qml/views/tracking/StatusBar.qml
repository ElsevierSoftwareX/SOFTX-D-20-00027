import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import imb.celltracker.data 1.0
import imb.celltracker.messagerelay 1.0

Item {
    RowLayout {
        Label {
            id: statusBar
            text: DataProvider.getStatus()
            Connections {
                target: MessageRelay
                onUpdateStatusBar: {
                    DataProvider.setStatus(message)
                }
            }
        }
    }
}
