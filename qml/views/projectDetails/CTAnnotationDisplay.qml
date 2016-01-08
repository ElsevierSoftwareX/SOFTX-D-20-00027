import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import imb.celltracker 1.0

Rectangle {
    id: leftSide
    color: "white"

    property string titleText: ""
    property int type: 0


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
            }

            ListModel { id: aModel }
            model: aModel

            Connections {
                target: DataProvider
                onAnnotationsChanged: tv.updateModel(type)
            }
        }
        RowLayout {
            id: bottomArea
            Layout.fillWidth: parent
            Layout.minimumHeight: addButton.height
            Layout.alignment: Qt.AlignRight

            Button {
                id: addButton
                enabled: GUIState.projPath != ""
                text: "add"
                onClicked: {
                    newAnnotationDialog.reset()
                    newAnnotationDialog.open()
                    tv.currentRow = tv.columnCount - 1
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

            Text {      Layout.fillWidth: parent; text: "Annotation Title" }
            TextField { Layout.fillWidth: parent; placeholderText: newAnnotationDialog.annotationTitleValue; id: atv }
            Text {      Layout.fillWidth: parent; text: "Annotation Description" }
            TextArea {  Layout.fillWidth: parent; text: newAnnotationDialog.annotationDescriptionValue; id: adv }
        }

        onAccepted: {
            var id = DataProvider.addAnnotation(type)
            DataProvider.changeAnnotation(id, type, atv.text, adv.text)
            tv.currentRow = tv.rowCount - 1
        }
    }
}
