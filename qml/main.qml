import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import imb.celltracker 1.0

Item {
    /* This is the main element containing the application window
       and the possible states of the project. The selected state
       determines, which view is currently shown. */
    id: mainItem
    state: stateList[0].stateName
    states: stateList

    /* not directly assigning the list of states works around a warning
     * in CTToolBar, that complains about states being non-NOTIFYable */
    property list<State> stateList: [
        State {
            name: "Tracking"
            property string iconPath: "///icons/icon_placeholder.png"
            property string stateName: "Tracking"
            PropertyChanges { target: mainItem; viewPath: "views/tracking/View.qml" }
        },
        State {
            name: "ProjectDetails"
            property string iconPath: "///icons/icon_placeholder.png"
            property string stateName: "ProjectDetails"
            PropertyChanges { target: mainItem; viewPath: "views/projectDetails/View.qml" }
        },
        State {
            name: "TestView"
            property string iconPath: "///icons/icon_placeholder.png"
            property string stateName: "TestView"
            PropertyChanges { target: mainItem; viewPath: "views/test/View.qml" }
        }
    ]

    property bool sidebarIsExpanded: true
    property string viewPath: "views/tracking/View.qml"
    property string toolBarPath: "CTToolBar.qml"
    property string statusBarPath: "CTStatusBar.qml"

    ApplicationWindow {
        id: window
        title: "CellTracker"
        visible: true
        width: 900
        height: 600

        menuBar: CTMenuBar {
            /* Loads the menu bar for all views. */
            id: menuBar
        }

        toolBar: Loader {
            /* Loads the tool bar of the current view. */
            id: currentToolBar
            source: mainItem.toolBarPath
        }

        Loader {
            /* Loads the main window of the current view. */
            id: currentMainWindow
            focus: true
            anchors.fill: parent
            source: mainItem.viewPath
        }

        statusBar: StatusBar {
            /* Loads the status bar of the current view. */
            id: currentStatusBar

            Loader {
                source: "CTStatusBar.qml"
            }
        }

        FileDialog {
            /* This dialog loads a chosen HDF5 file. */
            id: loadFileDialog
            folder: ""
            visible: false
            title: qsTr("Load project")
            selectExisting: true
            selectFolder: false
            selectMultiple: false
            onAccepted: {
                GUIState.projPath = loadFileDialog.fileUrl

                statusWindow.visible = true
                GUIState.mouseAreaActive = false
                DataProvider.loadHDF5(loadFileDialog.fileUrl)
            }
        }

        FileDialog {
            /* This dialog saves the project to a chosen HDF5 file. */
            id: saveFileDialog
            folder: ""
            visible: false
            title: qsTr("Save project")
            selectExisting: false
            selectFolder: false
            selectMultiple: false
            onAccepted: {
                statusWindow.visible = true
                GUIState.mouseAreaActive = false
                DataProvider.saveHDF5(saveFileDialog.fileUrl)
            }
        }
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
                folder: ""
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

    Window {
        id: statusWindow
        title: "Status"
        visible: false

        height: 100
        width: 300
        minimumHeight: height
        maximumHeight: height
        minimumWidth: width
        maximumWidth: width

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
                font.pointSize: CTSettings.value("text/status_fontsize")
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
                font.pointSize: CTSettings.value("text/status_fontsize")
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
                    statusWindow.visible = false
                    GUIState.mouseAreaActive = true
                }
            }
        }
    }
}
