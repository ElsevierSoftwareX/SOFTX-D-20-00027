import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.1
import imb.celltracker.data 1.0
import imb.celltracker.guistate 1.0

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

                Connections {
                    target: messageRelay
                    onFinishNotification: {
                        cellImage.source = "";
                        cellImage.source = "image://celltracking2/";
                    }
                }

                property bool isInTracklet: false
                property bool isSelectedInTracklet: false

                property int cellID: 0
                property int trackID: 0
                property int trackStart: 0
                property int trackEnd: 0
                property int trackLength: 0

                property int frameID: 0
                property int selectedCellID: 0
                property int selectedTrackID: 0
                property int selectedTrackStart: 0
                property int selectedTrackEnd: 0
                property int selectedTrackLength: 0

                property int jumpTrackEnd: 0
                property int frames: 2
                property real delay: 1.0

                property real offsetWidth: (width - paintedWidth) / 2
                property real offsetHeight: (height - paintedHeight) / 2

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    enabled: GUIState.getMouseAreaActive()

                    hoverEnabled: true
                    onClicked: {
                        GUIState.setLastX(mouseX - parent.offsetWidth)
                        GUIState.setLastY(mouseY - parent.offsetHeight)
                        GUIState.setMouseAction("leftClick")
                        slider.valueChanged()
                    }
                    onPositionChanged: {
                        if(focus == false) focus = true
                        GUIState.setLastX(mouseX - parent.offsetWidth)
                        GUIState.setLastY(mouseY - parent.offsetHeight)
                        GUIState.setMouseAction("hover")
                        slider.valueChanged()
                    }
                    focus: true
                    Keys.onPressed: {
                        GUIState.setMouseAction("hover")
                        if(event.key === Qt.Key_S) {
                            GUIState.setStatus("")
                            slider.value -= 1
                            event.accepted = true;
                        }
                        else if(event.key === Qt.Key_D) {
                            GUIState.setStatus("")
                            slider.value += 1
                            event.accepted = true;
                        }
                        else if(event.key === Qt.Key_A) {
                            GUIState.setStatus("")
                            slider.value -= 5
                            event.accepted = true;
                        }
                        else if(event.key === Qt.Key_F) {
                            GUIState.setStatus("")
                            slider.value += 5
                            event.accepted = true;
                        }
                        else if(event.key === Qt.Key_Space) {
                            if(GUIState.getStrategy() === "cell division") {
                                DataProvider.setStrategyStep(1)
                                DataProvider.setDaughterCells()
                                GUIState.setStrategy("")
                            }
                            else if(DataProvider.connectTracks()) {
                                GUIState.setLastX(mouseX - parent.offsetWidth)
                                GUIState.setLastY(mouseY - parent.offsetHeight)
                                GUIState.setMouseAction("leftClick")
                                slider.valueChanged()
                                GUIState.setMouseAction("hover")
                                slider.value += 1
                            }
                            event.accepted = true
                        }
                    }
                }
            }

            Slider {
                /* This is the slider element for navigating through the frames.
                   Reloads the image provider and updates the properties, if its
                   value or the mouse position has changed. */
                id: slider
                minimumValue: 1
                maximumValue: GUIState.getMaximumValue()
                value: 1
                stepSize: 1
                updateValueWhileDragging: true
                orientation: Qt.Horizontal
                anchors.rightMargin: 5
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: sliderValue.left
                }
                onValueChanged: {
                    if(GUIState.getPath() !== "") {
                        GUIState.setSliderValue(value)
                        cellImage.source = ""
                        cellImage.source = qsTr("image://celltracking2/")
                        cellImage.cellID = DataProvider.getCurrentObjectID()
                        if(cellImage.cellID != -1) {
                            cellImage.isInTracklet = DataProvider.isCurrentInTracklet()
                            if(cellImage.isInTracklet) {
                                cellImage.trackID = DataProvider.getCurrentTrackID()
                                cellImage.trackStart = DataProvider.getTrackStart(cellImage.trackID)
                                cellImage.trackEnd = DataProvider.getTrackEnd(cellImage.trackID)
                                cellImage.trackLength = DataProvider.getTrackLength(cellImage.trackID)
                            }
                            else {
                                cellImage.trackID = DataProvider.getCurrentAutoTrackID()
                                cellImage.trackStart = DataProvider.getAutoTrackStart(cellImage.trackID)
                                cellImage.trackEnd = DataProvider.getAutoTrackEnd(cellImage.trackID)
                                cellImage.trackLength = DataProvider.getAutoTrackLength(cellImage.trackID)
                            }
                        }
                        else {
                            cellImage.cellID = 0
                            cellImage.trackID = 0
                            cellImage.trackStart = 0
                            cellImage.trackEnd = 0
                            cellImage.trackLength = 0
                        }
                        if(GUIState.getMouseAction() === "leftClick") {
                            cellImage.selectedCellID = DataProvider.getSelectedObjectID()
                            if(cellImage.selectedCellID != -1) {
                                cellImage.frameID = value
                                cellImage.isSelectedInTracklet = DataProvider.isSelectedInTracklet()
                                if(cellImage.isSelectedInTracklet) {
                                    cellImage.selectedTrackID = DataProvider.getSelectedTrackID()
                                    cellImage.selectedTrackStart = DataProvider.getTrackStart(cellImage.selectedTrackID)
                                    cellImage.selectedTrackEnd = DataProvider.getTrackEnd(cellImage.selectedTrackID)
                                    cellImage.selectedTrackLength = DataProvider.getTrackLength(cellImage.selectedTrackID)
                                    cellImage.jumpTrackEnd = DataProvider.getAutoTrackEnd(DataProvider.getSelectedAutoTrackID())
                                }
                                else {
                                    cellImage.selectedTrackID = DataProvider.getSelectedAutoTrackID()
                                    cellImage.selectedTrackStart = DataProvider.getAutoTrackStart(cellImage.selectedTrackID)
                                    cellImage.selectedTrackEnd = DataProvider.getAutoTrackEnd(cellImage.selectedTrackID)
                                    cellImage.selectedTrackLength = DataProvider.getAutoTrackLength(cellImage.selectedTrackID)
                                    cellImage.jumpTrackEnd = cellImage.selectedTrackEnd
                                }
                            }
                            else {
                                cellImage.selectedCellID = 0
                                cellImage.frameID = 0
                                cellImage.selectedTrackID = 0
                                cellImage.selectedTrackStart = 0
                                cellImage.selectedTrackEnd = 0
                                cellImage.selectedTrackLength = 0
                                cellImage.jumpTrackEnd = 0
                            }
                        }
                        if(GUIState.getJumpStrategy() === "combine") {
                            GUIState.setJumpStrategy("")
                            GUIState.setMouseAction("hover")
                            if(cellImage.jumpTrackEnd - cellImage.frames > value)
                                value = cellImage.jumpTrackEnd - cellImage.frames
                            timer.interval = cellImage.delay * 1000
                            timer.running = true
                        }
                        else if(GUIState.getJumpStrategy() === "division") {
                            GUIState.setJumpStrategy("")
                            GUIState.setMouseAction("hover")
                            slider.value += 1
                        }
                    }
                }

                Timer {
                    id: timer
                    interval: 2000
                    running: false
                    repeat: false
                    onTriggered: {
                        slider.value += 1
                        if(slider.value <= cellImage.jumpTrackEnd)
                            running = true
                    }
                }
            }

            TextField {
                /* The field for editing the slider value. */
                id: sliderValue
                text: slider.value
                font.pixelSize: 14
                width: 40
                anchors.rightMargin: 5
                anchors {
                    bottom: parent.bottom
                    right: maximumValue.left
                }
                onEditingFinished: slider.value = text

                validator: IntValidator {
                    bottom: slider.minimumValue
                    top: slider.maximumValue
                }
            }

            Text {
                id: maximumValue
                text: "/%1".arg(slider.maximumValue)
                font.pixelSize: 14
                width: 30
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
                contentHeight: firstPanel.height + secondPanel.height
                    + thirdPanel.height + fourthPanel.height + fifthPanel.height
                    + sixthPanel.height
                anchors.fill: parent

                Loader {
                    id: firstPanel
                    source: "///qml/CollapsiblePanel.qml"
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                    }
                    onLoaded: {
                        item.titleText = "hovered object info"
                        item.state = "expanded"
                        item.model = cellInfoModel
                        item.delegate = cellInfoDelegate
                    }
                }

                ListModel {
                    id: cellInfoModel

                    ListElement {
                        text: "cell ID:"
                        autoText: "cell ID:"
                    }

                    ListElement {
                        text: "tracklet ID:"
                        autoText: "autotracklet ID:"
                    }

                    ListElement {
                        text: "tracklet start:"
                        autoText: "autotracklet start:"
                    }

                    ListElement {
                        text: "tracklet end:"
                        autoText: "autotracklet end:"
                    }

                    ListElement {
                        text: "tracklet length:"
                        autoText: "autotracklet length:"
                    }
                }

                Component {
                    id: cellInfoDelegate

                    Text {
                        text: cellImage.isInTracklet ? model.text : model.autoText
                        font.pixelSize: 12
                        width: 120

                        Text {
                            font.pixelSize: 12
                            anchors.left: parent.right
                            anchors.leftMargin: 5
                            text: {
                                switch(model.text) {
                                    case "cell ID:":
                                        cellImage.cellID
                                        break
                                    case "tracklet ID:":
                                        cellImage.trackID
                                        break
                                    case "tracklet start:":
                                        cellImage.trackStart
                                        break
                                    case "tracklet end:":
                                        cellImage.trackEnd
                                        break
                                    case "tracklet length:":
                                        cellImage.trackLength
                                        break
                                    default:
                                        ""
                                }
                            }
                        }
                    }
                }

                Loader {
                    id: secondPanel
                    source: "///qml/CollapsiblePanel.qml"
                    anchors {
                        top: firstPanel.bottom
                        left: parent.left
                        right: parent.right
                    }
                    onLoaded: {
                        item.titleText = "track info"
                        item.model = trackInfoModel
                        item.delegate = trackInfoDelegate
                    }
                }

                ListModel {
                    id: trackInfoModel

                    ListElement {
                        text: "current track:"
                    }

                    ListElement {
                        text: "start:"
                    }

                    ListElement {
                        text: "end:"
                    }

                    ListElement {
                        text: "length:"
                    }

                    ListElement {
                        text: "# cells:"
                    }

                    ListElement {
                        text: "mother track:"
                    }

                    ListElement {
                        text: "daughter tracks:"
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
                            text: ""
                        }
                    }
                }

                Loader {
                    id: thirdPanel
                    source: "///qml/CollapsiblePanel.qml"
                    anchors {
                        top: secondPanel.bottom
                        left: parent.left
                        right: parent.right
                    }
                    onLoaded: {
                        item.titleText = "selected object info"
                        item.state = "expanded"
                        item.model = selectedCellInfoModel
                        item.delegate = selectedCellDelegate
                    }
                }

                ListModel {
                    id: selectedCellInfoModel

                    ListElement {
                        text: "cell ID:"
                        autoText: "cell ID:"
                    }

                    ListElement {
                        text: "frame ID:"
                        autoText: "frame ID:"
                    }

                    ListElement {
                        text: "tracklet ID:"
                        autoText: "autotracklet ID:"
                    }

                    ListElement {
                        text: "tracklet start:"
                        autoText: "autotracklet start:"
                    }

                    ListElement {
                        text: "tracklet end:"
                        autoText: "autotracklet end:"
                    }

                    ListElement {
                        text: "tracklet length:"
                        autoText: "autotracklet length:"
                    }
                }

                Component {
                    id: selectedCellDelegate

                    Text {
                        text: cellImage.isSelectedInTracklet ? model.text : model.autoText
                        font.pixelSize: 12
                        width: 120

                        Text {
                            font.pixelSize: 12
                            anchors.left: parent.right
                            anchors.leftMargin: 5
                            text: {
                                switch(model.text) {
                                    case "cell ID:":
                                        cellImage.selectedCellID
                                        break
                                    case "frame ID:":
                                        cellImage.frameID
                                        break
                                    case "tracklet ID:":
                                        cellImage.selectedTrackID
                                        break
                                    case "tracklet start:":
                                        cellImage.selectedTrackStart
                                        break
                                    case "tracklet end:":
                                        cellImage.selectedTrackEnd
                                        break
                                    case "tracklet length:":
                                        cellImage.selectedTrackLength
                                        break
                                    default:
                                        ""
                                }
                            }
                        }
                    }
                }

                Loader {
                    id: fourthPanel
                    source: "///qml/CollapsiblePanel.qml"
                    anchors {
                        top: thirdPanel.bottom
                        left: parent.left
                        right: parent.right
                    }
                    onLoaded: {
                        item.titleText = "strategies"
                        item.footer = strategiesFooter
                        item.model = strategiesModel
                        item.delegate = strategiesDelegate
                    }
                }

                ListModel {
                    id: strategiesModel

                    ListElement {
                        text: "combine tracklets"
                    }

                    ListElement {
                        text: "cell division"
                    }

                    ListElement {
                        text: "change track status"
                    }
                }

                /*Component {
                    id: strategiesHeader

                    ComboBox {
                        width: 180
                        model: [
                            "click and step",
                            "combine tracklets",
                            "hover and step",
                            "click and jump",
                            "click and spin"
                        ]
                        onCurrentIndexChanged: {
                            GUIState.setStrategy(currentText)
                            switch(currentText) {
                                case "combine tracklets":
                                    GUIState.setStatus("Select cell object")
                                    break
                                default:
                                    GUIState.setStatus("")
                            }
                        }
                    }
                }*/

                Component {
                    id: strategiesDelegate

                    Button {
                        text: model.text
                        width: 160
                        onClicked: {
                            if(GUIState.getStrategy() === model.text) {
                                DataProvider.setStrategyStep(1)
                                GUIState.setStrategy("")
                                GUIState.setStatus("")
                                comboBox.visible = false
                            }
                            else {
                                GUIState.setStrategy(model.text)
                                switch(model.text) {
                                    case "combine tracklets":
                                        GUIState.setStatus("Select cell object")
                                        break
                                    case "cell division":
                                        DataProvider.setMotherCell()
                                        break
                                    case "change track status":
                                        comboBox.visible = true
                                        GUIState.setStatus("Select track object")
                                        break
                                    default:
                                        GUIState.setStatus("")
                                }
                            }
                        }

                        style: ButtonStyle {

                            label: Text {
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                font.pixelSize: 12
                                color: GUIState.getStrategy() === model.text ? "red" : "black"
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
                                DataProvider.setStatus(currentText)
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
                        visible: GUIState.getStrategy() === "combine tracklets" ? true : false

                        SpinBox {
                            width: 45
                            decimals: 0
                            minimumValue: 1
                            value: 2
                            stepSize: 1
                            anchors.left: parent.right
                            anchors.leftMargin: 5
                            onValueChanged: cellImage.frames = value - 1

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
                                    onValueChanged: cellImage.delay = value

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

                Loader {
                    id: fifthPanel
                    source: "///qml/CollapsiblePanel.qml"
                    anchors {
                        top: fourthPanel.bottom
                        left: parent.left
                        right: parent.right
                    }
                    onLoaded: {
                        item.titleText = "track operations"
                        item.model = trackOperationsModel
                        item.delegate = trackOperationsDelegate
                    }
                }

                ListModel {
                    id: trackOperationsModel

                    ListElement {
                        text: "current track"
                    }

                    ListElement {
                        text: "create new track"
                    }

                    ListElement {
                        text: "remove current track"
                        dialog: "removeCurrentTrackDialog"
                    }

                    ListElement {
                        text: "change tracks"
                    }

                    ListElement {
                        text: "add daughter track"
                    }

                    ListElement {
                        text: "remove daughter tracks"
                    }

                    ListElement {
                        text: "change track status"
                    }
                }

                Component {
                    id: trackOperationsDelegate

                    Button {
                        text: model.text
                        width: 160
                        onClicked: {
                            dialogLoader.sourceComponent = undefined
                            if(model.dialog === "removeCurrentTrackDialog") {
                                dialogLoader.sourceComponent = undefined
                                dialogLoader.sourceComponent = removeCurrentTrackDialog
                            }
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

                Loader {
                    id: sixthPanel
                    source: "///qml/CollapsiblePanel.qml"
                    anchors {
                        top: fifthPanel.bottom
                        left: parent.left
                        right: parent.right
                    }
                    onLoaded: {
                        item.titleText = "cell operations"
                        item.model = cellOperationsModel
                        item.delegate = cellOperationsDelegate
                    }
                }

                ListModel {
                    id: cellOperationsModel

                    ListElement {
                        text: "add cell"
                        dialog: ""
                    }

                    ListElement {
                        text: "remove cell"
                        dialog: ""
                    }

                    ListElement {
                        text: "remove all cells till now"
                        dialog: "removeTillNowDialog"
                    }

                    ListElement {
                        text: "remove all cells from now"
                        dialog: "removeFromNowDialog"
                    }
                }

                Component {
                    id: cellOperationsDelegate

                    Button {
                        text: model.text
                        width: 180
                        onClicked: {
                            dialogLoader.sourceComponent = undefined
                            switch(model.dialog) {
                                case "removeTillNowDialog":
                                    dialogLoader.sourceComponent = removeTillNowDialog
                                    break
                                case "removeFromNowDialog":
                                    dialogLoader.sourceComponent = removeFromNowDialog
                                    break
                            }
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
