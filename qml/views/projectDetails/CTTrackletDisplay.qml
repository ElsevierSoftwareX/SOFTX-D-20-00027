/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2016 Sebastian Wagner
 *
 * Celltracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Celltracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Celltracker.  If not, see <https://www.gnu.org/licenses/>.
 */
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
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
        }

        TableView {
            id: tv
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentRow:  -1
            frameVisible: true
            sortIndicatorVisible: true

            onSortIndicatorColumnChanged: tModel.sort()
            onSortIndicatorOrderChanged: tModel.sort()

            TableViewColumn { id: tvcId;       role: "id";        title: "ID";                   width: 40 }
            TableViewColumn { id: tvcStart;    role: "start";     title: "Start";                width: 40 }
            TableViewColumn { id: tvcEnd;      role: "end";       title: "End";                  width: 40 }
            TableViewColumn { id: tvcPrevious; role: "previous";  title: "Previous";             width: 100 }
            TableViewColumn { id: tvcNext;     role: "next";      title: "Next";                 width: 100 }
            TableViewColumn { id: tvcTAnno;    role: "tanno";     title: "Tracklet Annotations"; width: 150 }
            TableViewColumn { id: tvcOAnno;    role: "oanno";     title: "Object Annotations";   width: 150 }
            TableViewColumn { id: tvcStatus;   role: "status";    title: "Status";               width: 100 }

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
                                    "previous"  : m[i].previous,
                                    "next"      : m[i].next,
                                    "tanno"     : m[i].tanno,
                                    "oanno"     : m[i].oanno,
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
