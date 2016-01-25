import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import imb.celltracker 1.0
import "."

Rectangle {
    id: leftSide
    color: "white"

    property string titleText: ""

    function updateDisplay() {
        tv.updateModel()
    }

    ColumnLayout {
        id: wholeArea
        anchors.fill: parent
        anchors.margins: 5

        Text {
            text: titleText
            font.pixelSize: 16
            Layout.fillWidth: parent
            horizontalAlignment: Text.AlignHCenter
        }

        TableView {
            id: tv
            Layout.fillWidth: parent
            Layout.fillHeight: parent
            currentRow:  -1
            frameVisible: true
            sortIndicatorVisible: true

            onSortIndicatorColumnChanged: tModel.sort()
            onSortIndicatorOrderChanged: tModel.sort()

            TableViewColumn { id: tvcId;       role: "id";        title: "ID";        width: 50 }
            TableViewColumn { id: tvcStart;    role: "start";     title: "Start";     width: 50 }
            TableViewColumn { id: tvcEnd;      role: "end";       title: "End";       width: 50 }
            TableViewColumn { id: tvcMother;   role: "mother";    title: "Mother";    width: 50 }
            TableViewColumn { id: tvcDaughter; role: "daughters"; title: "Daughters"; width: 100 }
            TableViewColumn { id: tvcStatus;   role: "status";    title: "Status";    width: 100 }

            onDoubleClicked: {
                var item = tModel.get(row)
                GUIController.selectLastCellByTrackId(item["id"])
                GUIState.currentFrame = item["end"]
                mainItem.state = "Tracking"
            }

            function updateModel() {
                var m = DataProvider.tracklets

                tModel.clear()
                for (var i = 0; i< m.length; i++) {
                    tModel.append({ "id"        : m[i].id,
                                    "start"     : m[i].start,
                                    "end"       : m[i].end,
                                    "mother"    : m[i].mother,
                                    "daughters" : m[i].daughters,
                                    "status"    : m[i].status })
                }
                tModel.sort()
            }

            SortListModel {
                id: tModel
                sortColumnName: tv.getColumn(tv.sortIndicatorColumn).role
                sortOrder: tv.sortIndicatorOrder
            }

            model: tModel

            Connections {
                target: DataProvider
                onTrackletsChanged: tv.updateModel()
            }
        }
    }
}
