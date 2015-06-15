import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1

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
                mousePosition.path = loadFileDialog.fileUrl

                statusWindow.visible = true
                mousePosition.mouseAreaActive = false
                dataProvider.loadHDF5(loadFileDialog.fileUrl)
            }
        }

        Connections {
            target: messageRelay
            onFinishNotification: {
                statusWindow.visible = false
                mousePosition.mouseAreaActive = true
                mousePosition.maximumValue = dataProvider.getMaximumValue()
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
                dataProvider.saveHDF5(saveFileDialog.fileUrl)
                mousePosition.maximumValue = dataProvider.getMaximumValue()
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

            Item {
                /* This field contains all values that shall be used
                   by the image provider, for example mouse position
                   and mouse action. */
                id: mousePosition
                objectName: "mouseArea"

                property int maximumValue: 1
                property real lastX: 0
                property real lastY: 0
                property real sliderValue: 1
                property string status
                property string strategy
                property string jumpStrategy
                property string mouseAction
                property string path
                property bool mouseAreaActive: true
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
            width: 300
            height: 100

            property string overallName: ""
            property int overallMax: 0
            property int overallCurr: 0

            property string detailName: ""
            property int detailMax: 0
            property int detailCurr: 0

            ColumnLayout {
                anchors.fill: parent

                Label {
                    id: title
                    text: "Status"
                }

                Label {
                    id: overallNameField
                    text: "overallName"
                    Connections {
                        target: messageRelay
                        onUpdateOverallName: overallNameField.text = text
                    }
                }

                ProgressBar {
                    id: overallProgress
                    width: parent.width
                    value: 0
                    Connections {
                        target: messageRelay
                        onUpdateOverallMax: {
                            overallProgress.value = 0
                            overallProgress.maximumValue = newMax
                        }
                        onIncreaseOverall: overallProgress.value = overallProgress.value + 1
                    }
                }

                Label {
                    id: detailNameField
                    text: "detailName"
                    Connections {
                        target: messageRelay
                        onUpdateDetailName: detailNameField.text = text
                    }
                }

                ProgressBar {
                    id: detailProgress
                    value: 0
                    Connections {
                        target: messageRelay
                        onUpdateDetailMax: {
                            detailProgress.value = 0
                            detailProgress.maximumValue = newMax
                        }
                        onIncreaseDetail: detailProgress.value = detailProgress.value + 1
                    }
                }

                Connections {
                    target: messageRelay

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
