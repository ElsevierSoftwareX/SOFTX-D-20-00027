import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import imb.celltracker 1.0

Item {
    /* This is the main element containing the application window
       and the possible states of the project. The selected state
       determines, which view is currently shown. */
    id: mainItem
    state: "TrackingView"
    states: [

        State {
            name: "TrackingView"

            PropertyChanges {
                target: mainItem
                view: "views/tracking/View.qml"
                toolBar: "views/tracking/ToolBar.qml"
                //statusBar: "views/tracking/StatusBar.qml"
            }
        },

        State {
            name: "ProjectDetails"

            PropertyChanges {
                target: mainItem
                view: "views/projectDetails/View.qml"
                toolBar: "views/projectDetails/ToolBar.qml"
                //statusBar: "views/projectDetails/StatusBar.qml"
            }
        },

        State {
            name: "TestView"

            PropertyChanges {
                target: mainItem
                view: "views/test/View.qml"
                toolBar: "views/test/ToolBar.qml"
                //statusBar: "views/test/StatusBar.qml"
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
        menuBar: MyMenuBar {
            /* Loads the menu bar for all views. */
            id: menuBar
        }

        Dialog {
            /* This dialog can be used to create a new project. You
               can choose between XML and HDF5 import format. */
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
                    font.pixelSize: 12
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
                    font.pixelSize: 12
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
                    font.pixelSize: 12
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
            /* This dialog loads a chosen HDF5 file. */
            id: loadFileDialog
            folder: "."
            visible: false
            title: qsTr("Load project")
            selectExisting: true
            selectFolder: false
            selectMultiple: false
            onAccepted: {
                GUIState.path = loadFileDialog.fileUrl

                statusWindow.visible = true
                GUIState.mouseAreaActive = false
                DataProvider.loadHDF5(loadFileDialog.fileUrl)
            }
        }

        Connections {
            target: MessageRelay
            onFinishNotification: {
                statusWindow.visible = false
                GUIState.mouseAreaActive = true
                GUIState.maximumValue = DataProvider.getMaximumValue()
            }
        }

        FileDialog {
            /* This dialog saves the project to a chosen HDF5 file. */
            id: saveFileDialog
            folder: "."
            visible: false
            title: qsTr("Save project")
            selectExisting: false
            selectFolder: false
            selectMultiple: false
            onAccepted: {
                /*! \todo: save */
                DataProvider.saveHDF5(saveFileDialog.fileUrl)
                GUIState.maximumValue = DataProvider.getMaximumValue()
            }
        }

        toolBar: Loader {
            /* Loads the tool bar of the current view. */
            id: toolBar
            height: window.height / 15
            anchors.fill: parent
            source: mainItem.toolBar
        }

        Loader {
            /* Loads the main window of the current view. */
            focus: true
            height: window.contentItem.height
            source: mainItem.view
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
        }

        statusBar: StatusBar {
            /* Loads the status bar of the current view. */
            id: statusBar

            Loader {
                source: mainItem.statusBar
            }
        }

        Window {
            id: statusWindow
            title: "Status"
            visible: false

            height: 100
            width: 300

            x: window.x + (window.width/2) - (width/2)
            y: window.y + (window.height/2) - (height/2)

            property string overallName: ""
            property int overallMax: 0
            property int overallCurr: 0

            property string detailName: ""
            property int detailMax: 0
            property int detailCurr: 0

            GridLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                columns: 1

                Text {
                    id: overallNameField
                    anchors.left: parent.left
                    anchors.right: parent.right
                    width: 300

                    horizontalAlignment: Text.AlignHCenter
                    font.pointSize: CTSettings.value("text/status/size")
                    text: "overallName"
                    Connections {
                        target: MessageRelay
                        onUpdateOverallName: overallNameField.text = text
                    }
                }

                ProgressBar {
                    id: overallProgress
                    anchors.left: parent.left
                    anchors.right: parent.right
                    width: 300

                    value: 0
                    Connections {
                        target: MessageRelay
                        onUpdateOverallMax: {
                            overallProgress.value = 0
                            overallProgress.maximumValue = newMax
                        }
                        onIncreaseOverall: overallProgress.value = overallProgress.value + 1
                    }
                }

                Text {
                    id: detailNameField
                    anchors.left: parent.left
                    anchors.right: parent.right
                    width: 300

                    horizontalAlignment: Text.AlignHCenter
                    font.pointSize: CTSettings.value("text/status/size")
                    text: "detailName"
                    Connections {
                        target: MessageRelay
                        onUpdateDetailName: detailNameField.text = text
                    }
                }

                ProgressBar {
                    id: detailProgress
                    anchors.left: parent.left
                    anchors.right: parent.right
                    width: 300

                    value: 0
                    Connections {
                        target: MessageRelay
                        onUpdateDetailMax: {
                            detailProgress.value = 0
                            detailProgress.maximumValue = newMax
                        }
                        onIncreaseDetail: detailProgress.value = detailProgress.value + 1
                    }
                }

                Connections {
                    target: MessageRelay

                    onFinishNotification: {
                        overallNameField.text = ""
                        overallProgress.maximumValue = 0
                        overallProgress.value = 0
                        detailNameField.text = ""
                        detailProgress.maximumValue = 0
                        detailProgress.value = 0
                    }
                }
            }
        }
    }
}
