import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {

    Connections {
        id: conn
        target: MessageRelay
        property string overallName: ""
        property int overallMax: 0
        property int overallCurr: 0

        property string detailName: ""
        property int detailMax: 0
        property int detailCurr: 0

        onUpdateOverallName: {
            conn.overallName = text
            console.info(conn.overallName + ": " + conn.overallCurr + "/" + conn.overallMax + " " + conn.detailName + ": " + conn.detailCurr + "/" + conn.detailMax)
        }

        onUpdateOverallMax: {
            conn.overallMax = newMax
            conn.overallCurr = 0
            console.info(conn.overallName + ": " + conn.overallCurr + "/" + conn.overallMax + " " + conn.detailName + ": " + conn.detailCurr + "/" + conn.detailMax)
        }

        onIncreaseOverall: {
            conn.overallCurr = overallCurr + 1
            console.info(conn.overallName + ": " + conn.overallCurr + "/" + conn.overallMax + " " + conn.detailName + ": " + conn.detailCurr + "/" + conn.detailMax)
       }

        onUpdateDetailName: {
            conn.detailName = text
            console.info(conn.overallName + ": " + conn.overallCurr + "/" + conn.overallMax + " " + conn.detailName + ": " + conn.detailCurr + "/" + conn.detailMax)
      }

        onUpdateDetailMax: {
            conn.detailMax = newMax
            conn.detailCurr = 0
            console.info(conn.overallName + ": " + conn.overallCurr + "/" + conn.overallMax + " " + conn.detailName + ": " + conn.detailCurr + "/" + conn.detailMax)
     }

        onIncreaseDetail: {
            conn.detailCurr = detailCurr + 1
            console.info(conn.overallName + ": " + conn.overallCurr + "/" + conn.overallMax + " " + conn.detailName + ": " + conn.detailCurr + "/" + conn.detailMax)
    }

        onFinishNotification: {
            conn.overallName = ""
            conn.overallMax = 0
            conn.overallCurr = 0

            conn.detailName = ""
            conn.detailMax = 0
            conn.detailCurr = 0
            console.info(conn.overallName + ": " + conn.overallCurr + "/" + conn.overallMax + " " + conn.detailName + ": " + conn.detailCurr + "/" + conn.detailMax)
   }

        onUpdateStatusBar: {
            statusBar.text = message
        }
    }

    RowLayout {

        Label {
            id: statusBar
            text: mousePosition.status

            Connections {
                target: myImport
                onEventTriggered: statusBar.text = status
            }
        }
    }
}
