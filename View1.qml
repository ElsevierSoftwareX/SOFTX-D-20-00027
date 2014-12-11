import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Component {

    RowLayout {
        height: window.height * 0.9
        width: window.width

        Rectangle {
            color: window.color
            anchors {
                top: parent.top
                bottom: parent.bottom
                left: parent.left
                right: sidebar.left
            }

            Image {
                id: cellImage
                source: "img/smaller001.png"
                fillMode: Image.PreserveAspectFit
                anchors.fill: parent
                anchors.margins: 5
                /*anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: parent.right
                }*/
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

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 10

                GroupBox {
                    title: "cell operations"
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    Column {
                        spacing: 5

                        Button {
                            text: "add cell"

                            CheckBox {
                                text: "auto"
                                checked: true
                                anchors.left: parent.right
                                anchors.leftMargin: 5
                            }
                        }

                        Button {
                            text: "remove cell"
                        }

                        Button {
                            text: "remove all cells till now"
                        }

                        Button {
                            text: "remove all cells from now"
                        }
                    }
                }

                GroupBox {
                    title: "track operations"
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    Column {
                        spacing: 5

                        Text {
                            text: "current track:"

                        }

                        Button {
                            text: "create new track"
                        }

                        Button {
                            text: "remove current track"
                        }

                        Button {
                            text: "change tracks"
                        }

                        Button {
                            text: "add daughter track"
                        }

                        Button {
                            text: "remove daughter tracks"
                        }

                        Text {
                            text: "change track status:"

                        }
                    }
                }

                GroupBox {
                    title: "strategies"
                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    Column {
                        spacing: 5

                        Button {
                            text: "click and step"
                        }

                        Button {
                            text: "hover and step"
                        }

                        Button {
                            text: "click and jump"
                        }

                        Button {
                            text: "click and spin"
                        }
                    }
                }
            }
        }
    }
}
