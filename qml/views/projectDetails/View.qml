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

                                ListModel {
                                    id: lModel
                                    ListElement { title: "bla"; description: "nanana" }
                                    ListElement { title: "ble"; description: "nenene" }
                                    ListElement { title: "bli"; description: "ninini" }
                                    ListElement { title: "blu"; description: "nununu" }
                                    ListElement { title: "blo"; description: "nonono" }
                                }

                                ListView {
                                    id: lv
                                    height: 260
                                    width: 150
                                    orientation: ListView.Vertical
                                    currentIndex: -1

                                    model: DataProvider.annotations
                                    delegate: Rectangle {
                                        width: parent.width
                                        height: 20
                                        color: (lv.currentIndex === index)? "lightgray" : "white"
                                        Text {
                                            verticalAlignment: Text.AlignVCenter
                                            height: 20
                                            width: parent.width
                                            text: title
                                        }
                                        MouseArea {
                                            anchors.fill: parent
                                            onClicked: {
                                                /* update annotation info on the right side */
                                                lv.currentIndex = index
                                                console.log("highlighted: "+ index)
                                                rightSide.annotationTitleValue = title
                                                rightSide.annotationDescriptionValue = description
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        ColumnLayout {
                            id: rightSide
                            Layout.fillHeight: parent
                            Layout.fillWidth: parent

                            property string annotationTitleValue: "Annotation Title"
                            property string annotationDescriptionValue: "Annotation Description"

                            Text {
                                Layout.fillWidth: parent
                                text: "Annotation Title"
                            }
                            TextField {
                                Layout.fillWidth: parent
                                text: parent.annotationTitleValue
                            }
                            Text {
                                Layout.fillWidth: parent
                                text: "Annotation Description"
                            }
                            TextArea {
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
                            id: cancelButton
                            text: "cancel"
                        }
                        Button {
                            id: okButton
                            text: "ok"
                        }
                    }
                }

                //                ColumnLayout {
//                    anchors.fill: parent

//                    TableView {
//                        id: tableView
//                        model: DataProvider.annotationsModel
//                        anchors.fill: parent

//                        TableViewColumn {
//                            id: titleCol
//                            role: "title"
//                            title: "Title"
//                        }
//                        TableViewColumn {
//                            role: "description"
//                            title: "Description"
//                        }

//                    }
//                }
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
