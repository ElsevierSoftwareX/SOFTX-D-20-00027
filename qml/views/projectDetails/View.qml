import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import imb.celltracker 1.0

Item {

    RowLayout {
        anchors.fill: parent

        //        height: window.contentItem.height
        //        width: window.width

        Rectangle {
            id: leftSide
            color: window.color
            anchors {
                top: parent.top
                bottom: parent.bottom
                left: parent.left
                right: sidebar.left
            }

            Loader {
                id: loader
                sourceComponent: annotationsView
                anchors.fill: parent
            }

            Component {
                id: annotationsView

                ColumnLayout {
                    id: wholeArea
                    anchors.fill: parent
                    RowLayout {
                        id: topArea
                        Layout.fillHeight: parent
                        Layout.fillWidth: parent
                        Rectangle {
                            //                        Layout.preferredWidth: parent.width/3
                            width: 150
                            Layout.fillHeight: parent

                            ColumnLayout {
                                id: leftSide
                                Layout.fillHeight: parent
                                Layout.fillWidth: parent

                                ListView {
                                    id: lv
                                    height: 260
                                    width: 150
                                    orientation: ListView.Vertical
                                    currentIndex: -1

                                    function updateDisplay() {
                                        rightSide.annotationType = aModel[currentIndex].type
                                        rightSide.annotationId = aModel[currentIndex].id
                                        rightSide.annotationTitleValue = aModel[currentIndex].title
                                        rightSide.annotationDescriptionValue = aModel[currentIndex].description
                                    }

                                    property list<QtObject> aModel

                                    Component.onCompleted: aModel = DataProvider.annotations
                                    onCurrentIndexChanged: updateDisplay()
                                    Connections {
                                        target: DataProvider
                                        onAnnotationsChanged: lv.aModel = DataProvider.annotations
                                    }

                                    model: aModel
                                    delegate: Rectangle {
                                        width: parent.width
                                        height: 20
                                        color: (lv.currentIndex === index)? "lightgray" : "white"
                                        Text {
                                            verticalAlignment: Text.AlignVCenter
                                            height: 20
                                            width: parent.width
                                            text: model.title
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: lv.currentIndex = index
                                        }
                                    }

                                }
                            }
                        }
                        ColumnLayout {
                            id: rightSide
                            Layout.fillHeight: parent
                            Layout.fillWidth: parent

                            property int annotationId: -1
                            property string annotationTitleValue: "Annotation Title"
                            property string annotationDescriptionValue: "Annotation Description"
                            /* enum types are currently unsupported in qml and are mapped to int */
                            property int annotationType: -1

                            function reset() {
                                titleValue.text = annotationTitleValue
                                descriptionValue.text = annotationDescriptionValue
                            }

                            RowLayout {
                                ExclusiveGroup {
                                    id: typeGroup
                                }
                                RadioButton {
                                    id: objectType
                                    text: "Object"
                                    checked: rightSide.annotationType === Annotation.OBJECT_ANNOTATION
                                    onClicked: rightSide.annotationType = Annotation.OBJECT_ANNOTATION
                                    exclusiveGroup: typeGroup
                                }
                                RadioButton {
                                    id: trackletType
                                    text: "Tracklet"
                                    checked: rightSide.annotationType === Annotation.TRACKLET_ANNOTATION
                                    onClicked: rightSide.annotationType = Annotation.TRACKLET_ANNOTATION
                                    exclusiveGroup: typeGroup
                                }
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
                        Layout.minimumHeight: okButton.height
                        Layout.alignment: Qt.AlignRight

                        Button {
                            id: addButton
                            text: "add"
                            onClicked: {
                                DataProvider.addAnnotation(rightSide.annotationType)
                                lv.currentIndex = lv.aModel.length - 1
                            }
                        }
                        Button {
                            id: cancelButton
                            text: "cancel"
                            onClicked: {
                                rightSide.reset()
                                lv.updateDisplay()
                            }
                        }
                        Button {
                            id: okButton
                            text: "ok"
                            onClicked: {
                                var save = lv.currentIndex
                                DataProvider.changeAnnotation(rightSide.annotationId,
                                                              rightSide.annotationType,
                                                              titleValue.text,
                                                              descriptionValue.text)
                                lv.currentIndex = save
                            }
                        }
                    }
                }
            }
        }

        Rectangle {
            id: sidebar
            width: sidebarIsExpanded ? parent.width / 3 : 0
            color: Qt.rgba(0, 0, 0, 0.2)
            visible: sidebarIsExpanded ? true : false
            anchors {
                top: parent.top
                bottom: parent.bottom
                right: parent.right
            }

            GroupBox {
                anchors.margins: 20
                anchors.fill: parent

                Column {
                    spacing: 5
                    anchors.fill: parent

                    Button {
                        text: "Annotations"
                        anchors.left: parent.left
                        anchors.right: parent.right

                        onClicked: {
                            loader.sourceComponent = annotationsView
                        }
                    }

                    Button {
                        text: "Dummy"
                        anchors.left: parent.left
                        anchors.right: parent.right

                        onClicked: {
                            //                            loader.sourceComponent = timeView
                        }
                    }
                }
            }
        }
    }
}
