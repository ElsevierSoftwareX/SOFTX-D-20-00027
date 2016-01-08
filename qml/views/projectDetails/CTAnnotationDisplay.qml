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

        Text { text: titleText }

        RowLayout {
            id: topArea
            Layout.fillHeight: parent
            Layout.fillWidth: parent
            Rectangle {
                width: 150
                Layout.fillHeight: parent

                ColumnLayout {
                    id: annotationList
                    Layout.fillHeight: parent
                    Layout.fillWidth: parent

                    Rectangle {
                        width: parent.width
                        height: 20
                        color: "white"
                        visible: lv.count === 0
                        Text {
                            text: "No Annotations"
                            height: 20
                            width: 150
                            color: "gray"
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                        }
                    }

                    ListView {
                        id: lv
                        height: 260
                        width: 150
                        orientation: ListView.Vertical
                        currentIndex: -1

                        function updateModel(type) {
                            var m = DataProvider.annotations

                            aModel.clear()
                            for (var i = 0; i< m.length; i++)
                                if (m[i].type === type)
                                    aModel.append({ "id" : m[i].id,
                                                    "title" : m[i].title,
                                                    "description" : m[i].description })
                        }

                        function updateDisplay() {
                            var item = aModel.get(currentIndex)
                            if (!item) return
                            rightSide.annotationType = type
                            rightSide.annotationId = item.id
                            rightSide.annotationTitleValue = item.title
                            rightSide.annotationDescriptionValue = item.description
                        }

                        ListModel { id: aModel }
                        model: aModel

                        delegate: Rectangle {
                            width: parent.width
                            height: 20
                            color: (lv.currentIndex === index)? "lightgray" : "white"
                            Text {
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                height: 20
                                width: parent.width
                                text: model.title
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: lv.currentIndex = index
                            }
                        }

                        onCurrentIndexChanged: updateDisplay()
                        Connections {
                            target: DataProvider
                            onAnnotationsChanged: lv.updateModel(type)
                        }
                    }
                }
            }
            ColumnLayout {
                id: rightSide
                enabled: lv.count > 0
                Layout.fillHeight: parent
                Layout.fillWidth: parent

                property int annotationId: -1
                property string annotationTitleValue: "Annotation Title"
                property string annotationDescriptionValue: "Annotation Description"
                /* enum types are currently unsupported in qml and are mapped to int */
                property int annotationType: type

                function reset() {
                    titleValue.text = annotationTitleValue
                    descriptionValue.text = annotationDescriptionValue
                }

                Text {
                    Layout.fillWidth: parent
                    text: "Annotation Title"
                }
                TextField {
                    id: titleValue
                    Layout.fillWidth: parent
                    text: parent.annotationTitleValue
                }
                Text {
                    Layout.fillWidth: parent
                    text: "Annotation Description"
                }
                TextArea {
                    id: descriptionValue
                    Layout.fillWidth: parent
                    Layout.fillHeight: parent
                    text: parent.annotationDescriptionValue
                }
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
                    lv.currentIndex = lv.count - 1
                }
            }
            Button {
                id: deleteButton
                enabled: rightSide.annotationId != -1
                text: "delete"
                onClicked: {
                    var save = lv.currentIndex
                    DataProvider.deleteAnnotation(rightSide.annotationId)
                    if (save >= lv.count) lv.currentIndex = lv.count - 1
                    else lv.currentIndex = save
                    lv.updateDisplay()
                }
            }
        }
    }

    Dialog {
        id: newAnnotationDialog
        title: "New Annotation"
        height: 300
        width: 400

        standardButtons: StandardButton.Ok | StandardButton.Cancel

        property string annotationTitleValue: "New Annotation Title"
        property string annotationDescriptionValue: "New Annotation Description"

        function reset() {
            atv.text = "New Annotation Title"
            adv.text = "New Annotation Description"
        }

        ColumnLayout {
            anchors.fill: parent

            Text {      Layout.fillWidth: parent; text: "Annotation Title" }
            TextField { Layout.fillWidth: parent; text: newAnnotationDialog.annotationTitleValue; id: atv }
            Text {      Layout.fillWidth: parent; text: "Annotation Description" }
            TextArea {  Layout.fillWidth: parent; text: newAnnotationDialog.annotationDescriptionValue; id: adv }
        }

        onAccepted: {
            var id = DataProvider.addAnnotation(type)
            DataProvider.changeAnnotation(id, type, atv.text, adv.text)
            lv.currentIndex = lv.count - 1
        }
    }
}
