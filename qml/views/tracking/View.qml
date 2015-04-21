import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.1

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
                anchors {
                    top: parent.top
                    bottom: slider.top
                    left: parent.left
                    right: parent.right
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

                property int frames: 2
                property real delay: 1.0

                property real offsetWidth: (width - paintedWidth) / 2
                property real offsetHeight: (height - paintedHeight) / 2
                property real scaleFactor: sourceSize.width / paintedWidth

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        mousePosition.lastX = (mouseX - parent.offsetWidth) * parent.scaleFactor
                        mousePosition.lastY = (mouseY - parent.offsetHeight) * parent.scaleFactor
                        mousePosition.mouseAction = "leftClick"
                        slider.valueChanged()
                    }
                    onPositionChanged: {
                        if(focus == false) focus = true
                        mousePosition.lastX = (mouseX - parent.offsetWidth) * parent.scaleFactor
                        mousePosition.lastY = (mouseY - parent.offsetHeight) * parent.scaleFactor
                        mousePosition.mouseAction = "hover"
                        slider.valueChanged()
                    }
                    focus: true
                    Keys.onPressed: {
                        mousePosition.mouseAction = "hover"
                        if(event.key === Qt.Key_S) {
                            slider.value -= 1
                            event.accepted = true;
                        }
                        else if(event.key === Qt.Key_D) {
                            slider.value += 1
                            event.accepted = true;
                        }
                        else if(event.key === Qt.Key_A) {
                            slider.value -= 5
                            event.accepted = true;
                        }
                        else if(event.key === Qt.Key_F) {
                            slider.value += 5
                            event.accepted = true;
                        }
                        else if(event.key === Qt.Key_Space) {
                            if(myImport.connectTracks()) {
                                mousePosition.status = "Tracklets combined"
                                mousePosition.lastX = (mouseX - parent.offsetWidth) * parent.scaleFactor
                                mousePosition.lastY = (mouseY - parent.offsetHeight) * parent.scaleFactor
                                mousePosition.mouseAction = "leftClick"
                                slider.valueChanged()
                                mousePosition.mouseAction = "hover"
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
                maximumValue: mousePosition.maximumValue
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
                    if(mousePosition.path !== "") {
                        mousePosition.sliderValue = value
                        cellImage.source = ""
                        cellImage.source = qsTr("image://celltracking/")
                        cellImage.cellID = myImport.getCurrentObjectID()
                        if(cellImage.cellID != -1) {
                            cellImage.isInTracklet = myImport.isCurrentInTracklet()
                            if(cellImage.isInTracklet) {
                                cellImage.trackID = myImport.getCurrentTrackID()
                                cellImage.trackStart = myImport.getTrackStart(cellImage.trackID)
                                cellImage.trackEnd = myImport.getTrackEnd(cellImage.trackID)
                                cellImage.trackLength = myImport.getTrackLength(cellImage.trackID)
                            }
                            else {
                                cellImage.trackID = myImport.getCurrentAutoTrackID()
                                cellImage.trackStart = myImport.getAutoTrackStart(cellImage.trackID)
                                cellImage.trackEnd = myImport.getAutoTrackEnd(cellImage.trackID)
                                cellImage.trackLength = myImport.getAutoTrackLength(cellImage.trackID)
                            }
                        }
                        else {
                            cellImage.cellID = 0
                            cellImage.trackID = 0
                            cellImage.trackStart = 0
                            cellImage.trackEnd = 0
                            cellImage.trackLength = 0
                        }
                        if(mousePosition.mouseAction === "leftClick") {
                            cellImage.selectedCellID = myImport.getSelectedObjectID()
                            if(cellImage.selectedCellID != -1) {
                                cellImage.frameID = value
                                cellImage.isSelectedInTracklet = myImport.isSelectedInTracklet()
                                if(cellImage.isSelectedInTracklet) {
                                    cellImage.selectedTrackID = myImport.getSelectedTrackID()
                                    cellImage.selectedTrackStart = myImport.getTrackStart(cellImage.selectedTrackID)
                                    cellImage.selectedTrackEnd = myImport.getTrackEnd(cellImage.selectedTrackID)
                                    cellImage.selectedTrackLength = myImport.getTrackLength(cellImage.selectedTrackID)
                                }
                                else {
                                    cellImage.selectedTrackID = myImport.getSelectedAutoTrackID()
                                    cellImage.selectedTrackStart = myImport.getAutoTrackStart(cellImage.selectedTrackID)
                                    cellImage.selectedTrackEnd = myImport.getAutoTrackEnd(cellImage.selectedTrackID)
                                    cellImage.selectedTrackLength = myImport.getAutoTrackLength(cellImage.selectedTrackID)
                                }
                            }
                            else {
                                cellImage.selectedCellID = 0
                                cellImage.frameID = 0
                                cellImage.selectedTrackID = 0
                                cellImage.selectedTrackStart = 0
                                cellImage.selectedTrackEnd = 0
                                cellImage.selectedTrackLength = 0
                            }
                        }
                        if(mousePosition.jumpStrategy === "combine") {
                            mousePosition.jumpStrategy = ""
                            mousePosition.mouseAction = "hover"
                            if(cellImage.selectedTrackEnd - cellImage.frames > value)
                                value = cellImage.selectedTrackEnd - cellImage.frames
                            timer.interval = cellImage.delay * 1000
                            timer.running = true
                        }
                        else if(mousePosition.jumpStrategy === "division") {
                            mousePosition.jumpStrategy = ""
                            mousePosition.mouseAction = "hover"
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
                        if(slider.value <= cellImage.selectedTrackEnd)
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
                            mousePosition.strategy = currentText
                            switch(currentText) {
                                case "combine tracklets":
                                    mousePosition.status = "Select cell object"
                                    break
                                default:
                                    mousePosition.status = ""
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
                            if(mousePosition.strategy === model.text) {
                                myImport.setStrategyStep(1)
                                mousePosition.strategy = ""
                                mousePosition.status = ""
                                comboBox.visible = false
                            }
                            else {
                                mousePosition.strategy = model.text
                                switch(model.text) {
                                    case "combine tracklets":
                                        mousePosition.status = "Select cell object"
                                        break
                                    case "cell division":
                                        mousePosition.status = "Select mother track"
                                        break
                                    case "change track status":
                                        comboBox.visible = true
                                        mousePosition.status = "Select track object"
                                        break
                                    default:
                                        mousePosition.status = ""
                                }
                            }
                        }

                        style: ButtonStyle {

                            label: Text {
                                verticalAlignment: Text.AlignVCenter
                                horizontalAlignment: Text.AlignHCenter
                                font.pixelSize: 12
                                color: mousePosition.strategy === model.text ? "red" : "black"
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
                                myImport.setStatus(currentText)
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
                        visible: mousePosition.strategy === "combine tracklets" ? true : false

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
