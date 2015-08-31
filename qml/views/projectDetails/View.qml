import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
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
                    anchors.fill: parent

                    TableView {
                        id: tableView
                        model: DataProvider.annotationsModel
                        anchors.fill: parent

                        TableViewColumn {
                            id: titleCol
                            role: "title"
                            title: "Title"
                        }
                        TableViewColumn {
                            role: "description"
                            title: "Description"
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
