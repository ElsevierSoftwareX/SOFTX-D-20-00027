import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import imb.celltracker 1.0

Item {
    /* This is the element for showing the main window of the tracking
       view. It consists of a workspace area and a sidebar. */

    RowLayout {
        height: window.contentItem.height
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
                /* This is the image element for drawing the frames. It contains
                   properties that shall be shown in the sidebar. The mouse area
                   triggers an event to reload the image provider, when the user
                   has moved the cursor. */
                id: cellImage
                cache: false
                fillMode: Image.PreserveAspectFit
                anchors.margins: 5
                sourceSize.width: width
                sourceSize.height: height

                anchors {
                    top: parent.top
                    bottom: slider.top
                    left: parent.left
                    right: parent.right
                }

                function updateImage() {
                    cellImage.source = "";
                    cellImage.source = "image://celltracking2/";
                }

                Connections {
                    target: MessageRelay
                    onFinishNotification: cellImage.updateImage()
                }
                Connections {
                    target: GUIState
                    onNewCurrentFrameChanged: cellImage.updateImage()
                }

                property real offsetWidth: (width - paintedWidth) / 2
                property real offsetHeight: (height - paintedHeight) / 2

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    enabled: GUIState.newMouseAreaActive

                    hoverEnabled: true
                    onClicked: {
                        GUIState.newMouseX = (mouseX - parent.offsetWidth)
                        GUIState.newMouseY = (mouseY - parent.offsetHeight)
                        GUIController.selectCell(GUIState.newCurrentFrame, GUIState.newMouseX, GUIState.newMouseY)
                        cellImage.updateImage()
                    }
                    onPositionChanged: {
                        GUIState.newMouseX = (mouseX - parent.offsetWidth)
                        GUIState.newMouseY = (mouseY - parent.offsetHeight)
                        GUIController.hoverCell(GUIState.newCurrentFrame, GUIState.newMouseX, GUIState.newMouseY)
                        cellImage.updateImage()
                    }
                    focus: true
                    Keys.onPressed: {
                        switch (event.key) {
                            case Qt.Key_A: slider.value -= 5;
                                break;
                            case Qt.Key_S: slider.value -= 1;
                                break;
                            case Qt.Key_D: slider.value += 1;
                                break;
                            case Qt.Key_F: slider.value += 5;
                                break;
                            case Qt.Key_Space:
                                /* todo: select cell */
                                switch (GUIController.currentAction) {
                                case GUIState.ACTION_DEFAULT:
                                    GUIController.connectTracks();
                                    break;
                                case GUIState.ACTION_ADD_DAUGHTERS:
                                    GUIController.changeAction(GUIState.ACTION_DEFAULT);
                                    break;
                                }
                                slider.value += 1; /* always? */
                                break;
                        }
//                        else if(event.key === Qt.Key_Space) {
//                            if(GUIState.strategy === "cell division") {
//                                DataProvider.setStrategyStep(1)
//                                DataProvider.setDaughterCells()
//                                GUIState.strategy = ""
//                            }
//                            else if(DataProvider.connectTracks()) {
//                                GUIState.lastX = (mouseX - parent.offsetWidth)
//                                GUIState.lastY = (mouseY - parent.offsetHeight)
//                                GUIState.mouseAction = "leftClick"
//                                slider.valueChanged()
//                                GUIState.mouseAction = "hover"
//                                slider.value += 1
//                            }
//                            event.accepted = true
//                        }
                    }
                }
            }

            Slider {
                /* This is the slider element for navigating through the frames.
                   Reloads the image provider and updates the properties, if its
                   value or the mouse position has changed. */
                id: slider
                minimumValue: 0
                maximumValue: GUIState.newMaximumFrame
                value: 0
                stepSize: 1
                updateValueWhileDragging: true
                orientation: Qt.Horizontal
                anchors.rightMargin: 5
                anchors {
                    bottom: parent.bottom
                    left: parent.left
//                    right: sliderValue.left
                    right: currentFrameDisplay.left
                }

                onValueChanged: GUIController.changeFrameAbs(value)

//                    GUIState.newCurrentFrame = value - 1
//                    if(GUIState.path !== "") {
//                        GUIState.currentFrame = value - 1
//                        cellImage.source = ""
//                        cellImage.source = qsTr("image://celltracking2/")
//                        cellImage.cellID = DataProvider.getCurrentObjectID()
//                        if(cellImage.cellID != -1) {
//                            cellImage.isInTracklet = DataProvider.isCurrentInTracklet()
//                            if(cellImage.isInTracklet) {
//                                cellImage.trackID = DataProvider.getCurrentTrackID()
//                                cellImage.trackStart = DataProvider.getTrackStart(cellImage.trackID)
//                                cellImage.trackEnd = DataProvider.getTrackEnd(cellImage.trackID)
//                                cellImage.trackLength = DataProvider.getTrackLength(cellImage.trackID)
//                            }
//                            else {
//                                cellImage.trackID = DataProvider.getCurrentAutoTrackID()
//                                cellImage.trackStart = DataProvider.getAutoTrackStart(cellImage.trackID)
//                                cellImage.trackEnd = DataProvider.getAutoTrackEnd(cellImage.trackID)
//                                cellImage.trackLength = DataProvider.getAutoTrackLength(cellImage.trackID)
//                            }
//                        }
//                        else {
//                            cellImage.cellID = 0
//                            cellImage.trackID = 0
//                            cellImage.trackStart = 0
//                            cellImage.trackEnd = 0
//                            cellImage.trackLength = 0
//                        }
//                        if(GUIState.mouseAction === "leftClick") {
//                            cellImage.selectedCellID = DataProvider.getSelectedObjectID()
//                            if(cellImage.selectedCellID != -1) {
//                                cellImage.frameID = value
//                                cellImage.isSelectedInTracklet = DataProvider.isSelectedInTracklet()
//                                if(cellImage.isSelectedInTracklet) {
//                                    cellImage.selectedTrackID = DataProvider.getSelectedTrackID()
//                                    cellImage.selectedTrackStart = DataProvider.getTrackStart(cellImage.selectedTrackID)
//                                    cellImage.selectedTrackEnd = DataProvider.getTrackEnd(cellImage.selectedTrackID)
//                                    cellImage.selectedTrackLength = DataProvider.getTrackLength(cellImage.selectedTrackID)
//                                    cellImage.jumpTrackEnd = DataProvider.getAutoTrackEnd(DataProvider.getSelectedAutoTrackID())
//                                }
//                                else {
//                                    cellImage.selectedTrackID = DataProvider.getSelectedAutoTrackID()
//                                    cellImage.selectedTrackStart = DataProvider.getAutoTrackStart(cellImage.selectedTrackID)
//                                    cellImage.selectedTrackEnd = DataProvider.getAutoTrackEnd(cellImage.selectedTrackID)
//                                    cellImage.selectedTrackLength = DataProvider.getAutoTrackLength(cellImage.selectedTrackID)
//                                    cellImage.jumpTrackEnd = cellImage.selectedTrackEnd
//                                }
//                            }
//                            else {
//                                cellImage.selectedCellID = 0
//                                cellImage.frameID = 0
//                                cellImage.selectedTrackID = 0
//                                cellImage.selectedTrackStart = 0
//                                cellImage.selectedTrackEnd = 0
//                                cellImage.selectedTrackLength = 0
//                                cellImage.jumpTrackEnd = 0
//                            }
//                        }
//                        if(GUIState.jumpStrategy === "combine") {
//                            GUIState.jumpStrategy = ""
//                            GUIState.mouseAction = "hover"
//                            if(cellImage.jumpTrackEnd - cellImage.frames > value)
//                                value = cellImage.jumpTrackEnd - cellImage.frames
//                            timer.interval = cellImage.delay * 1000
//                            timer.running = true
//                        }
//                        else if(GUIState.jumpStrategy === "division") {
//                            GUIState.jumpStrategy = ""
//                            GUIState.mouseAction = "hover"
//                            slider.value += 1
//                        }
//                    }
//                }

                Timer {
                    id: timer
                    interval: 2000
                    running: false
                    repeat: false
                    onTriggered: {
//                        slider.value += 1
//                        if(slider.value <= cellImage.jumpTrackEnd)
//                            running = true
                    }
                }
            }

//            TextField {
//                /* The field for editing the slider value. */
//                id: sliderValue
//                text: slider.value
//                font.pixelSize: 14
//                width: 40
//                anchors.rightMargin: 5
//                anchors {
//                    bottom: parent.bottom
//                    right: maximumValue.left
//                }
//                onEditingFinished: slider.value = text

//                validator: IntValidator {
//                    bottom: slider.minimumValue
//                    top: slider.maximumValue
//                }
//            }

            Text {
                id: currentFrameDisplay
                text: "%1/%2".arg(slider.value + 1).arg(slider.maximumValue + 1)
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 14
                width: 60
                anchors {
                    bottom: parent.bottom
                    right: parent.right
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

            Loader {
                /* Loads the dialogs created by the delegates. */
                id: dialogLoader
            }

            Flickable {
                /* This is a flickable element that arranges the collapsible panels
                   in the sidebar. Each panel needs a model for showing information
                   and a delegate to implement the functionality. */
                contentHeight: cellInfo.height + trackInfo.height
                    + selectedInfo.height + actionsPanel.height + strategiesPanel.height + trackOperations.height
                    + cellOperations.height
                anchors.fill: parent
                id: flick

                /* ================= Panel cellInfo ================= */
                property list<QtObject> cellInfoModel: [
                    QtObject { property string text: "cell ID"; property int value: GUIState.newHoveredCellID },
                    QtObject { property string text: "frame ID"; property int value: GUIState.newCurrentFrame },
                    QtObject { property string text: "autoTracklet ID"; property int value: GUIState.newHoveredAutoTrackID },
                    QtObject { property string text: "autoTracklet start"; property int value: GUIState.newHoveredAutoTrackStart },
                    QtObject { property string text: "autoTracklet end"; property int value: GUIState.newHoveredAutoTrackEnd },
                    QtObject { property string text: "autoTracklet length"; property int value: GUIState.newHoveredAutoTrackLength },
                    QtObject { property string text: "tracklet ID"; property int value: GUIState.newHoveredTrackID },
                    QtObject { property string text: "tracklet start"; property int value: GUIState.newHoveredTrackStart },
                    QtObject { property string text: "tracklet end"; property int value: GUIState.newHoveredTrackEnd },
                    QtObject { property string text: "tracklet length"; property int value: GUIState.newHoveredTrackLength }
                ]

                Loader {
                    id: cellInfo
                    source: "///qml/CollapsiblePanel.qml"
                    anchors { top: parent.top; left: parent.left; right: parent.right }
                    onLoaded: {
                        item.titleText = "hovered object info"
                        item.state = "expanded"
                        item.model = flick.cellInfoModel
                        item.delegate = cellInfoDelegate
                    }
                }

                Component {
                    id: cellInfoDelegate

                    Text {
                        text: model.text
                        font.pixelSize: 12
                        width: 120

                        Text {
                            font.pixelSize: 12
                            anchors.left: parent.right
                            anchors.leftMargin: 5
                            text: model.value
                        }
                    }
                }

                /* ================= Panel trackInfo ================= */
                property list<QtObject> trackInfoModel: [
                    QtObject { property string text: "current track:"; property string value: "placeholder" },
                    QtObject { property string text: "start:"; property string value: "placeholder" },
                    QtObject { property string text: "end:"; property string value: "placeholder" },
                    QtObject { property string text: "length:"; property string value: "placeholder" },
                    QtObject { property string text: "# cells:"; property string value: "placeholder" },
                    QtObject { property string text: "mother track:"; property string value: "placeholder" },
                    QtObject { property string text: "daughter tracks:"; property string value: "placeholder" }
                ]

                Loader {
                    id: trackInfo
                    source: "///qml/CollapsiblePanel.qml"
                    anchors {
                        top: cellInfo.bottom
                        left: parent.left
                        right: parent.right
                    }
                    onLoaded: {
                        item.titleText = "track info"
                        item.model = flick.trackInfoModel
                        item.delegate = trackInfoDelegate
                    }
                }

                Component {
                    id: trackInfoDelegate

                    Text {
                        text: model.text
                        font.pixelSize: 12
                        width: 120

                        Text {
                            font.pixelSize: 12
                            anchors.left: parent.right
                            anchors.leftMargin: 5
                            text: model.value
                        }
                    }
                }

                /* ================= Panel selectedInfo ================= */
                property list<QtObject> selectedCellModel: [
                    QtObject { property string text: "cell ID"; property int value: GUIState.newSelectedCellID },
                    QtObject { property string text: "frame ID"; property int value: GUIState.newCurrentFrame },
                    QtObject { property string text: "autoTracklet ID"; property int value: GUIState.newSelectedAutoTrackID },
                    QtObject { property string text: "autoTracklet start"; property int value: GUIState.newSelectedAutoTrackStart },
                    QtObject { property string text: "autoTracklet end"; property int value: GUIState.newSelectedAutoTrackEnd },
                    QtObject { property string text: "autoTracklet length"; property int value: GUIState.newSelectedAutoTrackLength },
                    QtObject { property string text: "tracklet ID"; property int value: GUIState.newSelectedTrackID },
                    QtObject { property string text: "tracklet start"; property int value: GUIState.newSelectedTrackStart },
                    QtObject { property string text: "tracklet end"; property int value: GUIState.newSelectedTrackEnd },
                    QtObject { property string text: "tracklet length"; property int value: GUIState.newSelectedTrackLength }
                ]

                Loader {
                    id: selectedInfo
                    source: "///qml/CollapsiblePanel.qml"
                    anchors { top: trackInfo.bottom; left: parent.left; right: parent.right }
                    onLoaded: {
                        item.titleText = "selected object info"
                        item.state = "expanded"
                        item.model = flick.selectedCellModel
                        item.delegate = selectedCellDelegate
                    }
                }

                Component {
                    id: selectedCellDelegate

                    Text {
                        text: model.text
                        font.pixelSize: 12
                        width: 120

                        Text {
                            font.pixelSize: 12
                            anchors.left: parent.right
                            anchors.leftMargin: 5
                            text: model.value
                        }
                    }
                }

                /* ================= Panel actionsPanel ================= */
                property list<QtObject> actionsModel: [
                    QtObject { property string text: "add daughters"; property int val: GUIState.ACTION_ADD_DAUGHTERS },
                    QtObject { property string text: "delete cell"; property int val: GUIState.ACTION_DELETE_CELL }
                ]

                Loader {
                    id: actionsPanel
                    source: "///qml/CollapsiblePanel.qml"
                    anchors { top: selectedInfo.bottom; left: parent.left; right: parent.right }
                    onLoaded: {
                        item.titleText = "actions"
                        item.footer = actionsFooter
                        item.model = flick.actionsModel
                        item.delegate = actionsDelegate
                    }
                }

                Component {
                    id: actionsDelegate

                    Button {
                        text: model.text
                        width: 160
                        onClicked: {
                            /* toggle between action */
                            if (GUIController.currentAction === model.val)
                                GUIController.changeAction(GUIState.ACTION_DEFAULT);
                            else
                                GUIController.changeAction(model.val);

//                            if(GUIState.strategy === model.text) {
//                                DataProvider.setStrategyStep(1)
//                                GUIState.strategy = ""
//                                GUIState.status = ""
//                                comboBox.visible = false
//                            }
//                            else {
//                                GUIState.strategy = model.text
//                                switch(model.text) {
//                                    case "combine tracklets":
//                                        GUIState.status = "Select cell object"
//                                        break
//                                    case "cell division":
//                                        DataProvider.setMotherCell()
//                                        break
//                                    case "change track status":
//                                        comboBox.visible = true
//                                        GUIState.status = "Select track object"
//                                        break
//                                    default:
//                                        GUIState.status = ""
//                                }
//                            }
                        }

                        style: ButtonStyle {

                            label: Text {
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                font.pixelSize: 12
                                color: GUIController.currentAction === model.val ? "red" : "black"
                                text: control.text
                            }
                        }

                        ComboBox {
                            id: comboBox
                            width: 120
                            model: ["open", "cell division", "dead", "lost", "end of movie reached"]
                            anchors.left: parent.right
                            anchors.leftMargin: 5
                            visible: model.text === "change track status"
                            onCurrentIndexChanged: {
//                                DataProvider.setStatus(currentText)
                            }
                        }
                    }
                }

                Component {
                    id: actionsFooter

                    Text {
                        text: "show last frames:"
                        font.pixelSize: 12
                        width: 110
                        visible: GUIState.strategy === "combine tracklets" ? true : false

                        SpinBox {
                            width: 45
                            decimals: 0
                            minimumValue: 1
                            value: 2
                            stepSize: 1
                            anchors.left: parent.right
                            anchors.leftMargin: 5
                            onValueChanged: GUIState.frames = value - 1

                            Text {
                                text: "delay time:"
                                font.pixelSize: 12
                                width: 65
                                anchors.left: parent.right
                                anchors.leftMargin: 5

                                SpinBox {
                                    width: 50
                                    decimals: 1
                                    minimumValue: 0
                                    value: 1.0
                                    stepSize: 0.1
                                    anchors.left: parent.right
                                    anchors.leftMargin: 5
                                    onValueChanged: GUIState.delay = value

                                    /*CheckBox {
                                        text: "no double"
                                        checked: true
                                        anchors.left: parent.right
                                        anchors.leftMargin: 5
                                    }*/
                                }
                            }
                        }
                    }
                }

                /* ================= Panel strategiesPanel ================= */
                property list<QtObject> strategiesModel: [
                    QtObject { property string text: "click & jump"; property int val: GUIState.STRATEGY_CLICK_JUMP },
                    QtObject { property string text: "click & spin"; property int val: GUIState.STRATEGY_CLICK_SPIN },
                    QtObject { property string text: "click & step"; property int val: GUIState.STRATEGY_CLICK_STEP },
                    QtObject { property string text: "hover & step"; property int val: GUIState.STRATEGY_HOVER_STEP }
//                    QtObject { property string text: "combine tracklets" },
//                    QtObject { property string text: "cell division" },
//                    QtObject { property string text: "change track status" }
                ]

                Loader {
                    id: strategiesPanel
                    source: "///qml/CollapsiblePanel.qml"
                    anchors { top: actionsPanel.bottom; left: parent.left; right: parent.right }
                    onLoaded: {
                        item.titleText = "strategies"
                        item.footer = strategiesFooter
                        item.model = flick.strategiesModel
                        item.delegate = strategiesDelegate
                    }
                }

                Component {
                    id: strategiesDelegate

                    Button {
                        text: model.text
                        width: 160
                        onClicked: {
                            GUIController.changeStrategy(model.val);
                            GUIController.startStrategy();
//                            if(GUIState.strategy === model.text) {
//                                DataProvider.setStrategyStep(1)
//                                GUIState.strategy = ""
//                                GUIState.status = ""
//                                comboBox.visible = false
//                            }
//                            else {
//                                GUIState.strategy = model.text
//                                switch(model.text) {
//                                    case "combine tracklets":
//                                        GUIState.status = "Select cell object"
//                                        break
//                                    case "cell division":
//                                        DataProvider.setMotherCell()
//                                        break
//                                    case "change track status":
//                                        comboBox.visible = true
//                                        GUIState.status = "Select track object"
//                                        break
//                                    default:
//                                        GUIState.status = ""
//                                }
//                            }
                        }

                        style: ButtonStyle {

                            label: Text {
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                font.pixelSize: 12
                                color: GUIState.strategy === model.text ? "red" : "black"
                                text: control.text
                            }
                        }

                        ComboBox {
                            id: comboBox
                            width: 120
                            model: ["open", "cell division", "dead", "lost", "end of movie reached"]
                            anchors.left: parent.right
                            anchors.leftMargin: 5
                            visible: model.text === "change track status"
                            onCurrentIndexChanged: {
//                                DataProvider.setStatus(currentText)
                            }
                        }
                    }
                }

                Component {
                    id: strategiesFooter

                    Text {
                        text: "show last frames:"
                        font.pixelSize: 12
                        width: 110
                        visible: GUIState.strategy === "combine tracklets" ? true : false

                        SpinBox {
                            width: 45
                            decimals: 0
                            minimumValue: 1
                            value: 2
                            stepSize: 1
                            anchors.left: parent.right
                            anchors.leftMargin: 5
                            onValueChanged: GUIState.frames = value - 1

                            Text {
                                text: "delay time:"
                                font.pixelSize: 12
                                width: 65
                                anchors.left: parent.right
                                anchors.leftMargin: 5

                                SpinBox {
                                    width: 50
                                    decimals: 1
                                    minimumValue: 0
                                    value: 1.0
                                    stepSize: 0.1
                                    anchors.left: parent.right
                                    anchors.leftMargin: 5
                                    onValueChanged: GUIState.delay = value

                                    /*CheckBox {
                                        text: "no double"
                                        checked: true
                                        anchors.left: parent.right
                                        anchors.leftMargin: 5
                                    }*/
                                }
                            }
                        }
                    }
                }

                /* ================= Panel trackOperations ================= */
                property list<QtObject> trackOperationsModel: [
                    QtObject { property string text: "current track" },
                    QtObject { property string text: "create new track" },
                    QtObject { property string text: "remove current track"; property Component dialog: removeCurrentTrackDialog },
                    QtObject { property string text: "change tracks" },
                    QtObject { property string text: "add daughter track" },
                    QtObject { property string text: "remove daughter tracks" },
                    QtObject { property string text: "change track status " }
                ]

                Loader {
                    id: trackOperations
                    source: "///qml/CollapsiblePanel.qml"
                    anchors { top: strategiesPanel.bottom; left: parent.left; right: parent.right }
                    onLoaded: {
                        item.titleText = "track operations"
                        item.model = flick.trackOperationsModel
                        item.delegate = trackOperationsDelegate
                    }
                }

                Component {
                    id: trackOperationsDelegate

                    Button {
                        text: model.text
                        width: 160
                        onClicked: {
                            if(model.dialog)
                                dialogLoader.sourceComponent = model.dialog
                        }

                        ComboBox {
                            width: 120
                            model: ["open", "cell division", "dead", "lost", "end of movie reached"]
                            anchors.left: parent.right
                            anchors.leftMargin: 5

                            CheckBox {
                                text: "no double"
                                checked: true
                                anchors.left: parent.right
                                anchors.leftMargin: 5
                            }
                        }
                    }
                }

                Component {
                    id: removeCurrentTrackDialog

                    MessageDialog {
                        icon: StandardIcon.Information
                        text: "Do you want to remove current track?"
                        standardButtons: StandardButton.Yes | StandardButton.No
                        Component.onCompleted: visible = true
                        onYes: console.log("yes")
                        onNo: console.log("no")
                    }
                }

                /* ================= Panel cellOperations ================= */
                property list<QtObject> cellOperationsModel: [
                    QtObject { property string text: "add cell" },
                    QtObject { property string text: "remove cell" },
                    QtObject { property string text: "remove all cells till now"; property Component dialog: removeTillNowDialog },
                    QtObject { property string text: "remove all cells from now"; property Component dialog: removeFromNowDialog }
                ]

                Loader {
                    id: cellOperations
                    source: "///qml/CollapsiblePanel.qml"
                    anchors {
                        top: trackOperations.bottom
                        left: parent.left
                        right: parent.right
                    }
                    onLoaded: {
                        item.titleText = "cell operations"
                        item.model = flick.cellOperationsModel
                        item.delegate = cellOperationsDelegate
                    }
                }

                Component {
                    id: cellOperationsDelegate

                    Button {
                        text: model.text
                        width: 180
                        onClicked: {
                           if (model.dialog)
                               dialogLoader.sourceComponent = model.dialog
                        }
                    }
                }

                Component {
                    id: removeTillNowDialog

                    MessageDialog {
                        icon: StandardIcon.Information
                        text: "Do you want to remove all cells till now?"
                        standardButtons: StandardButton.Yes | StandardButton.No
                        Component.onCompleted: visible = true
                        onYes: console.log("yes")
                        onNo: console.log("no")
                    }
                 }

                Component {
                    id: removeFromNowDialog

                    MessageDialog {
                        icon: StandardIcon.Information
                        text: "Do you want to remove all cells from now?"
                        standardButtons: StandardButton.Yes | StandardButton.No
                        Component.onCompleted: visible = true
                        onYes: console.log("yes")
                        onNo: console.log("no")
                    }
                }
            }
        }
    }
}
