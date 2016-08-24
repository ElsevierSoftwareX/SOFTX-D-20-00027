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
    property int type: 0

    function updateDisplay() {
        tv.updateModel(type)
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

            onSortIndicatorColumnChanged: aModel.sort()
            onSortIndicatorOrderChanged: aModel.sort()

            TableViewColumn { id: tvcI; role: "id";          title: "ID";          width: 50 }
            TableViewColumn { id: tvcT; role: "title";       title: "Title";       width: tv.viewport.width * 0.3 }
            TableViewColumn { id: tvcD; role: "description"; title: "Description"; width: tv.viewport.width - tvcI.width - tvcT.width}

            function updateModel(type) {
                var m = DataProvider.annotations

                aModel.clear()
                for (var i = 0; i< m.length; i++)
                    if (m[i].type === type)
                        aModel.append({ "id" : m[i].id,
                                        "title" : m[i].title,
                                        "description" : m[i].description })
                aModel.sort()
            }

            SortListModel {
                id: aModel
                sortColumnName: tv.getColumn(tv.sortIndicatorColumn).role
                sortOrder: tv.sortIndicatorOrder
            }
            model: aModel

            Connections {
                target: DataProvider
                onAnnotationsChanged: tv.updateModel(type)
            }
        }
        RowLayout {
            id: bottomArea
            Layout.fillWidth: true
            Layout.minimumHeight: addButton.height
            Layout.alignment: Qt.AlignRight

            Button {
                id: addButton
                enabled: GUIState.projPath != ""
                text: "add"
                onClicked: {
                    newAnnotationDialog.reset()
                    newAnnotationDialog.open()
                    tv.currentRow = tv.rowCount - 1
                }
            }
            Button {
                id: editButton
                enabled: tv.currentRow != -1 && GUIState.projPath != ""
                text: "edit"
                onClicked: {
                    var idx = tv.currentRow
                    var item = tv.model.get(idx)
                    if (!item) return

                    editAnnotationDialog.reset(item["id"], item["title"], item["description"])
                    editAnnotationDialog.open()
                }
            }
            Button {
                id: deleteButton
                enabled: tv.currentRow != -1 && GUIState.projPath != ""
                text: "delete"
                onClicked: {
                    var save = tv.currentRow
                    var item = tv.model.get(save)
                    if (!item) return
                    var id = item["id"]
                    DataProvider.deleteAnnotation(id)
                    if (save >= tv.rowCount) tv.currentRow = tv.rowCount - 1
                    else tv.currentRow = save
                }
            }
        }
    }

    Dialog {
        id: newAnnotationDialog
        title: "New " + ((type == Annotation.OBJECT_ANNOTATION)?"Object":"Tracklet") + " Annotation"
        height: 300
        width: 400

        standardButtons: StandardButton.Ok | StandardButton.Cancel

        property string annotationTitleValue: "New Annotation Title"
        property string annotationDescriptionValue: "New Annotation Description"

        function reset() {
            atv.text = ""
            adv.text = ""
        }

        ColumnLayout {
            anchors.fill: parent

            Text {      Layout.fillWidth: true; text: "Annotation Title" }
            TextField { Layout.fillWidth: true; placeholderText: newAnnotationDialog.annotationTitleValue; id: atv }
            Text {      Layout.fillWidth: true; text: "Annotation Description" }
            TextArea {  Layout.fillWidth: true; text: newAnnotationDialog.annotationDescriptionValue; id: adv }
        }

        onAccepted: {
            var id = DataProvider.addAnnotation(type)
            DataProvider.changeAnnotation(id, type, atv.text, adv.text)
            tv.currentRow = tv.rowCount - 1
        }
    }

    Dialog {
        id: editAnnotationDialog
        title: "Edit " + ((type == Annotation.OBJECT_ANNOTATION)?"Object":"Tracklet") + " Annotation"
        height: 300
        width: 400

        standardButtons: StandardButton.Ok | StandardButton.Cancel

        property int annotationId: -1
        property string annotationTitleValue: "New Annotation Title"
        property string annotationDescriptionValue: "New Annotation Description"

        function reset(id, title, description) {
            annotationId = id
            eatv.text = title
            eadv.text = description
        }

        ColumnLayout {
            anchors.fill: parent

            Text {      Layout.fillWidth: true; text: "Annotation Title" }
            TextField { Layout.fillWidth: true; placeholderText: editAnnotationDialog.annotationTitleValue; id: eatv }
            Text {      Layout.fillWidth: true; text: "Annotation Description" }
            TextArea {  Layout.fillWidth: true; text: editAnnotationDialog.annotationDescriptionValue; id: eadv }
        }

        onAccepted: {
            var save = tv.currentRow
            DataProvider.changeAnnotation(annotationId, type, eatv.text, eadv.text)
            tv.currentRow = save
        }
    }
}
