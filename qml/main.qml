import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1
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
            name: "Project"
            property string iconPath: "///icons/icon_placeholder.png"
            property string stateName: "Project"
            PropertyChanges { target: mainItem; viewPath: "views/projectDetails/View.qml" }
        },
        State {
            name: "Outlines"
            property string iconPath: "///icons/icon_placeholder.png"
            property string stateName: "Outlines"
            PropertyChanges { target: mainItem; viewPath: "views/outlines/View.qml" }
        },
        State {
            name: "Config"
            property string iconPath: "///icons/icon_placeholder.png"
            property string stateName: "Config"
            PropertyChanges { target: mainItem; viewPath: "views/configuration/View.qml" }
        }
    ]

    property bool sidebarIsExpanded: true
    property string viewPath: "views/tracking/View.qml"
    property string toolBarPath: "CTToolBar.qml"
    property string statusBarPath: "CTStatusBar.qml"
    property int fileType: GUIState.PROJTYPE_HDF5

    ApplicationWindow {
        id: window
        title: "CellTracker" + ((GUIState.projPath === "")?"":" — " + DataProvider.localFileFromURL(GUIState.projPath))
        visible: true
        width: 900
        height: 600

        onClosing: if (GUIState.projPath !== "") { saveAndQuitDialog.open() }

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

            onStatusChanged: if (status === Loader.Ready) item.viewActivationHook()
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
            selectFolder: GUIState.projType !== GUIState.PROJTYPE_HDF5
            selectMultiple: false
            onAccepted: {
                GUIState.projPath = loadFileDialog.fileUrl

                statusWindow.visible = true
                GUIState.mouseAreaActive = false

                if (GUIState.projType === GUIState.PROJTYPE_HDF5)
                    DataProvider.loadHDF5(loadFileDialog.fileUrl)
                else
                    DataProvider.loadXML(loadFileDialog.fileUrl)
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

    MessageDialog {
        id: saveAndQuitDialog
        visible: false
        icon: StandardIcon.Question
        text: "Do you want to save the current project before quitting?"
        standardButtons: StandardButton.Yes | StandardButton.No | StandardButton.Close
        onRejected: saveAndQuitDialog.close()
        onYes: {
            DataProvider.saveHDF5()
            Qt.quit()
        }
        onNo: {
            Qt.quit()
        }
    }

    MessageDialog {
        id: saveAndOpenDialog
        visible: false
        icon: StandardIcon.Question
        text: "Do you want to save the current project before opening another one?"
        standardButtons: StandardButton.Yes | StandardButton.No | StandardButton.Close
        onRejected: saveAndOpenDialog.close()
        onYes: {
            DataProvider.saveHDF5()
            loadFileDialog.open()
        }
        onNo: {
            loadFileDialog.open()
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

    Dialog {
        id: exportDialog
        visible: false
        title: "Export Project"

        height: 225
        width: 350

        property string projPath: GUIState.projPath
        property string savePath: (GUIState.projType === GUIState.PROJTYPE_HDF5)?
                                      projPath.substring(0, projPath.length-3)+"-export.h5":
                                      projPath + "-export.h5"

        function checkCombination() {
            var path = (filename.text === "")?exportDialog.savePath:filename.text;
            var sane = DataProvider.sanityCheckOptions(
                        path,
                        sAnnotations.checked,
                        sAutoTracklets.checked,
                        sTracklets.checked,
//                        sEvents.checked,
                        sImages.checked,
                        sInfo.checked,
                        sObjects.checked,
                        sTracklets.checked);
            return sane;
        }

        function save() {
            if (exportDialog.checkCombination()) {
                var path = (filename.text === "")?exportDialog.savePath:filename.text

                var s_Annotations = sAnnotations.checked
                var s_AutoTracklets = sAutoTracklets.checked
//                var s_Events = sEvents.checked
                var s_Images = sImages.checked
                var s_Info = sInfo.checked
                var s_Objects = sObjects.checked
                var s_Tracklets = sTracklets.checked

                statusWindow.visible = true
                GUIState.mouseAreaActive = false
                DataProvider.saveHDF5(path, s_Annotations, s_AutoTracklets, s_Tracklets, s_Images, s_Info, s_Objects, s_Tracklets);
//                DataProvider.saveHDF5(path, s_Annotations, s_AutoTracklets, s_Events, s_Images, s_Info, s_Objects, s_Tracklets);
                exportDialog.close()
            }
        }

        contentItem: Rectangle {
            anchors.fill: parent

            ColumnLayout {
                anchors.fill: parent
                Text {
                    Layout.fillWidth: true
                    text: "Save:"
                    horizontalAlignment: Text.AlignHCenter
                }

                GridLayout {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    anchors.margins: 5
                    columns: 2


                    CheckBox { id: sAnnotations;   text: "Annotations";   onCheckedChanged: if (!exportDialog.checkCombination()) checked = !checked }
                    CheckBox { id: sAutoTracklets; text: "AutoTracklets"; onCheckedChanged: if (!exportDialog.checkCombination()) checked = !checked }
//                    CheckBox { id: sEvents;        text: "Events";        onCheckedChanged: if (!exportDialog.checkCombination()) checked = !checked }
                    CheckBox { id: sImages;        text: "Images";        onCheckedChanged: if (!exportDialog.checkCombination()) checked = !checked }
                    CheckBox { id: sInfo;          text: "Info";          onCheckedChanged: if (!exportDialog.checkCombination()) checked = !checked }
                    CheckBox { id: sObjects;       text: "Objects";       onCheckedChanged: if (!exportDialog.checkCombination()) checked = !checked }
                    CheckBox { id: sTracklets;     text: "Tracklets";     onCheckedChanged: if (!exportDialog.checkCombination()) checked = !checked }
                }
                RowLayout {
                    Layout.fillWidth: true
                    TextField {
                        id: filename
                        Layout.fillWidth: true
                        placeholderText: DataProvider.localFileFromURL(exportDialog.savePath)
                        onAccepted: exportDialog.save()
                    }
                    Button {
                        implicitWidth: height
                        text: "..."
                        onClicked: {
                            exportFileDialog.open()
                        }
                        FileDialog {
                            id: exportFileDialog
                            selectExisting: false
                            selectFolder: false
                            selectMultiple: false
                            visible: false

                            onAccepted: filename.text = exportFileDialog.fileUrl
                        }
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignRight
                    Button {
                        text: "Cancel"
                        onClicked: exportDialog.close()
                    }
                    Button {
                        id: saveButton
                        text: "Save"
                        onClicked: exportDialog.save()
                    }
                }
            }
        }

    }

    Window {
        id: statusWindow
        title: "Status"
        visible: false
        modality: Qt.ApplicationModal

        height: 100
        width: 300
        minimumHeight: height
        maximumHeight: height
        minimumWidth: width
        maximumWidth: width

        x: window.x + (window.width/2) - (width/2)
        y: window.y + (window.height/2) - (height/2)

        property string overallName: "overallName"
        property int overallMax: 0
        property int overallCurr: 0

        property string detailName: "detailName"
        property int detailMax: 0
        property int detailCurr: 0

        Connections {
            target: MessageRelay
            onUpdateOverallName: { statusWindow.overallName = text }
            onUpdateOverallMax: { statusWindow.overallCurr = 0; statusWindow.overallMax = newMax }
            onIncreaseOverall: { statusWindow.overallCurr++ }
            onUpdateDetailName: { statusWindow.detailName = text }
            onUpdateDetailMax: { statusWindow.detailCurr = 0; statusWindow.detailMax = newMax }
            onIncreaseDetail: { statusWindow.detailCurr++ }
            onFinishNotification: {
                statusWindow.overallName = ""
                statusWindow.overallMax = 0
                statusWindow.overallCurr = 0
                statusWindow.detailName = ""
                statusWindow.detailMax = 0
                statusWindow.detailCurr = 0
                statusWindow.visible = false
                GUIState.mouseAreaActive = true
            }
 }

        GridLayout {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 5
            columns: 1

            Text {
                id: overallNameField
                anchors.left: parent.left
                anchors.right: parent.right
                width: 300

                horizontalAlignment: Text.AlignHCenter
                font.pointSize: CTSettings.value("text/status_fontsize")
                text: statusWindow.overallName
            }

            ProgressBar {
                id: overallProgress
                anchors.left: parent.left
                anchors.right: parent.right
                width: 300

                value: statusWindow.overallCurr
                maximumValue: statusWindow.overallMax
            }

            Text {
                id: detailNameField
                anchors.left: parent.left
                anchors.right: parent.right
                width: 300

                horizontalAlignment: Text.AlignHCenter
                font.pointSize: CTSettings.value("text/status_fontsize")
                text: statusWindow.detailName
            }

            ProgressBar {
                id: detailProgress
                anchors.left: parent.left
                anchors.right: parent.right
                width: 300

                value: statusWindow.detailCurr
                maximumValue: statusWindow.detailMax

            }
        }
    }

    CTImport {
        id: importDialog
    }
}
