import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
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

                property int cellID: 0
                property int trackID: 0
                property int trackStart: 0
                property int trackEnd: 0
                property int trackLength: 0

                property int selectedCellID: 0
                property int selectedTrackID: 0
                property int selectedTrackStart: 0
                property int selectedTrackEnd: 0
                property int selectedTrackLength: 0

                property int frames: 0
                property real delay: 0

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
                        mousePosition.lastX = (mouseX - parent.offsetWidth) * parent.scaleFactor
                        mousePosition.lastY = (mouseY - parent.offsetHeight) * parent.scaleFactor
                        mousePosition.mouseAction = "hover"
                        slider.valueChanged()
                    }
                    focus: true
                    Keys.onPressed: {
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
                            if(myImport.connectTracks())
                                mousePosition.status = "Tracklets combined"
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
                        cellImage.cellID = myImport.getObjectID()
                        cellImage.trackID = myImport.getTrackID()
                        if(cellImage.trackID != -1) {
                            cellImage.trackStart = myImport.getTrackStart(cellImage.trackID)
                            cellImage.trackEnd = myImport.getTrackEnd(cellImage.trackID)
                            cellImage.trackLength = myImport.getTrackLength(cellImage.trackID)
                        }
                        else {
                            cellImage.trackStart = 0
                            cellImage.trackEnd = 0
                            cellImage.trackLength = 0
                        }
                        if(mousePosition.mouseAction === "leftClick") {
                            cellImage.selectedCellID = cellImage.cellID
                            cellImage.selectedTrackID = cellImage.trackID
                            cellImage.selectedTrackStart = cellImage.trackStart
                            cellImage.selectedTrackEnd = cellImage.trackEnd
                            cellImage.selectedTrackLength = cellImage.trackLength
                        }
                        if(mousePosition.jumpFrames > 0) {
                            mousePosition.jumpFrames = 0
                            mousePosition.mouseAction = "hover"
                            if(cellImage.trackEnd - cellImage.frames > value)
                                value = cellImage.trackEnd - cellImage.frames
                            timer.interval = cellImage.delay * 1000
                            timer.running = true
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
                        if(slider.value <= cellImage.trackEnd)
                            running = true
                    }
                }
            }

            TextField {
                /* The field for editing the slider value. */
                id: sliderValue
                text: slider.value
                font.pointSize: 14
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
                font.pointSize: 14
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
                        item.titleText = "object info"
                        item.model = cellInfoModel
                        item.delegate = textDelegate
                    }
                }

                ListModel {
                    id: cellInfoModel

                    ListElement {
                        text: "cell ID:"
                        value: ""
                    }

                    ListElement {
                        text: "track ID:"
                        value: ""
                    }

                    ListElement {
                        text: "track start:"
                        value: ""
                    }

                    ListElement {
                        text: "track end:"
                        value: ""
                    }

                    ListElement {
                        text: "track length:"
                        value: ""
                    }
                }

                Component {
                    id: textDelegate

                    Text {
                        text: model.text
                        font.pointSize: 12
                        width: 120

                        Text {
                            font.pointSize: 12
                            anchors.left: parent.right
                            anchors.leftMargin: 5
                            text: {
                                switch(model.text) {
                                    case "cell ID:":
                                        cellImage.cellID
                                        break
                                    case "track ID:":
                                        cellImage.trackID
                                        break
                                    case "track start:":
                                        cellImage.trackStart
                                        break
                                    case "track end:":
                                        cellImage.trackEnd
                                        break
                                    case "track length:":
                                        cellImage.trackLength
                                        break
                                    case "selected cell ID:":
                                        cellImage.selectedCellID
                                        break
                                    case "selected track ID:":
                                        cellImage.selectedTrackID
                                        break
                                    case "selected track start:":
                                        cellImage.selectedTrackStart
                                        break
                                    case "selected track end:":
                                        cellImage.selectedTrackEnd
                                        break
                                    case "selected track length:":
                                        cellImage.selectedTrackLength
                                        break
                                    default:
                                        model.value
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
                        item.delegate = textDelegate
                    }
                }

                ListModel {
                    id: trackInfoModel

                    ListElement {
                        text: "current track:"
                        value: "0"
                    }

                    ListElement {
                        text: "start:"
                        value: "0"
                    }

                    ListElement {
                        text: "end:"
                        value: "123"
                    }

                    ListElement {
                        text: "length:"
                        value: "123"
                    }

                    ListElement {
                        text: "# cells:"
                        value: "123"
                    }

                    ListElement {
                        text: "mother track:"
                        value: ""
                    }

                    ListElement {
                        text: "daughter tracks:"
                        value: ""
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
                        item.model = selectedCellInfoModel
                        item.delegate = textDelegate
                    }
                }

                ListModel {
                    id: selectedCellInfoModel

                    ListElement {
                        text: "selected cell ID:"
                        value: ""
                    }

                    ListElement {
                        text: "selected track ID:"
                        value: ""
                    }

                    ListElement {
                        text: "selected track start:"
                        value: ""
                    }

                    ListElement {
                        text: "selected track end:"
                        value: ""
                    }

                    ListElement {
                        text: "selected track length:"
                        value: ""
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
                        width: 180
                        onClicked: {
                            if(mousePosition.strategy === model.text) {
                                myImport.setLastObjectID(-1)
                                mousePosition.strategy = ""
                                mousePosition.status = ""
                            }
                            else {
                                mousePosition.strategy = model.text
                                switch(model.text) {
                                    case "combine tracklets":
                                        mousePosition.status = "Select cell object"
                                        break
                                    default:
                                        mousePosition.status = ""
                                }
                            }
                        }
                    }
                }

                Component {
                    id: strategiesFooter

                    Text {
                        text: "show last frames:"
                        font.pointSize: 12
                        width: 110
                        visible: mousePosition.strategy === "combine tracklets" ? true : false

                        SpinBox {
                            width: 45
                            decimals: 0
                            minimumValue: 1
                            stepSize: 1
                            anchors.left: parent.right
                            anchors.leftMargin: 5
                            onValueChanged: cellImage.frames = value - 1

                            Text {
                                text: "delay time:"
                                font.pointSize: 12
                                width: 65
                                anchors.left: parent.right
                                anchors.leftMargin: 5

                                SpinBox {
                                    width: 50
                                    decimals: 1
                                    minimumValue: 0
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
