import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1

Item {
    id: mainItem
    state: "TrackingView"
    states: [

        State {
            name: "TrackingView"

            PropertyChanges {
                target: mainItem
                view: "views/tracking/View.qml"
                toolBar: "views/tracking/ToolBar.qml"
                statusBar: "views/tracking/StatusBar.qml"
            }
        },

        State {
            name: "ProjectDetails"

            PropertyChanges {
                target: mainItem
                view: "views/projectDetails/View.qml"
                toolBar: "views/projectDetails/ToolBar.qml"
                statusBar: "views/projectDetails/StatusBar.qml"
            }
        },

        State {
            name: "TestView"

            PropertyChanges {
                target: mainItem
                view: "views/test/View.qml"
                toolBar: "views/test/ToolBar.qml"
                statusBar: "views/test/StatusBar.qml"
            }
        }
    ]

    property bool sidebarIsExpanded: true
    property string toolBar: "views/tracking/ToolBar.qml"
    property string view: "views/tracking/View.qml"
    property string statusBar: "views/tracking/StatusBar.qml"

    ApplicationWindow {
        id: window
        title: "CellTracker"
        visible: true
        width: 900
        height: 600

        Loader {
            source: "MenuBar.qml"
        }

        // Auswahl Importformat XML,HDF5, Button NEXT
        // bei XML: Projectname: Eingabefeld
        // Projectfolder: Auswahlmenü (FileOpnDialog nur Ordner), OK; CANCEL
        // bei HDF5: Projectfile: Eingabe (SELECT) OK; CANCEL

        //property string

        Dialog {
            id: dialog
            visible: false
            title: "Choose import format"

            contentItem: Rectangle {
                implicitWidth: 400
                implicitHeight: 200

                ComboBox {
                    id: comboBox
                    width: 120
                    model: ["XML", "HDF5"]
                    anchors.margins: 5
                    anchors {
                        top: parent.top
                        left: parent.left
                    }
                }

                Text {
                    id: text
                    text: "Project name:"
                    font.pointSize: 12
                    width: 120
                    height: 25
                    visible: comboBox.currentText == "XML" ? true : false
                    anchors.margins: 5
                    anchors {
                        top: comboBox.bottom
                        left: parent.left
                    }

                    TextField {
                        width: 120
                        placeholderText: qsTr("Enter name")
                        anchors.left: parent.right
                        anchors.leftMargin: 5
                    }
                }

                Text {
                    text: "Project folder:"
                    font.pointSize: 12
                    width: 120
                    height: 25
                    visible: comboBox.currentText == "XML" ? true : false
                    anchors.margins: 5
                    anchors {
                        top: text.bottom
                        left: parent.left
                    }

                    Button {
                        text: qsTr("Choose")
                        width: 120
                        anchors.left: parent.right
                        anchors.leftMargin: 5
                        onClicked: {
                            folderDialog.visible = true
                        }
                    }
                }

                FileDialog {
                    id: folderDialog
                    folder: "file:///Volumes/imbcloud.medizin.tu-dresden.de"
                    visible: false
                    title: qsTr("Open folder")
                    selectExisting: true
                    selectFolder: false
                    selectMultiple: false
                    onAccepted: {
                        console.log("You chose: " + folderDialog.fileUrl)
                    }
                }

                Text {
                    text: "Project file:"
                    font.pointSize: 12
                    width: 120
                    height: 25
                    visible: comboBox.currentText == "HDF5" ? true : false
                    anchors.margins: 5
                    anchors {
                        top: comboBox.bottom
                        left: parent.left
                    }

                    TextField {
                        width: 120
                        placeholderText: qsTr("Enter name")
                        anchors.left: parent.right
                        anchors.leftMargin: 5
                    }
                }

                Button {
                    text: "Ok"
                    width: 80
                    anchors.margins: 5
                    anchors {
                        bottom: parent.bottom
                        right: parent.right
                    }
                    onClicked: {
                        dialog.visible = false
                    }

                    Button {
                        text: "Cancel"
                        width: 80
                        anchors.right: parent.left
                        anchors.rightMargin: 5
                    }
                }
            }
        }

        FileDialog {
            id: fileDialog
            folder: "file:///Volumes/imbcloud.medizin.tu-dresden.de"
            visible: false
            title: qsTr("Open file")
            selectExisting: true
            selectFolder: false
            selectMultiple: false
            onAccepted: {
                console.log("You chose: " + fileDialog.fileUrl)
                myClass.loadHDF5(fileDialog.fileUrl)
                mousePosition.path = fileDialog.fileUrl
                mousePosition.maximumValue = 26
            }
        }

        toolBar: Loader {
            anchors.fill: parent
            source: mainItem.toolBar
        }

        Loader {
            height: window.height * 0.9 - statusBar.height
            source: mainItem.view
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }

            Item {
                id: mousePosition
                objectName: "mouseArea"

                property int maximumValue: 1
                property real lastX: 0
                property real lastY: 0
                property real sliderValue
                property string mouseAction
                property string path
            }
        }

        statusBar: StatusBar {
            id: statusBar

            Loader {
                source: mainItem.statusBar
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }
            }
        }
    }
}
