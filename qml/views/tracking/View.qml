import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.1

Item {

    RowLayout {
        height: window.height - toolBar.height - statusBar.height
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
                cache: false
                fillMode: Image.PreserveAspectFit
                anchors.margins: 5
                anchors {
                    top: parent.top
                    bottom: slider.top
                    left: parent.left
                    right: parent.right
                }

                property real offsetWidth: (width - paintedWidth) / 2
                property real offsetHeight: (height - paintedHeight) / 2
                property real scaleFactor: sourceSize.width / paintedWidth
                //property string path: "file:///Users/enrico/Downloads/students/example data/example data/img"

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
                }
            }

            Slider {
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
                        /*var filename = ""
                        if(value < 10) filename = "smaller00%1.png".arg(value)
                        else if(value < 100) filename = "smaller0%1.png".arg(value)
                        else filename = "smaller%1.png".arg(value)
                        cellImage.source = qsTr("image://celltracking/%1/%2").arg(cellImage.path).arg(filename)*/
                    }
                }
            }

            TextField {
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
                id: dialogLoader
            }

            Flickable {
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
                        value: "1234"
                    }

                    ListElement {
                        text: "track ID:"
                        value: "12"
                    }

                    ListElement {
                        text: "start:"
                        value: "123"
                    }

                    ListElement {
                        text: "end:"
                        value: "123"
                    }

                    ListElement {
                        text: "length:"
                        value: "12"
                    }
                }

                Component {
                    id: textDelegate

                    Text {
                        text: model.text
                        font.pointSize: 12
                        width: 120

                        Text {
                            text: model.value
                            font.pointSize: 12
                            anchors.left: parent.right
                            anchors.leftMargin: 5
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
                        item.titleText = "strategies"
                        item.header = strategiesHeader
                        item.model = strategiesModel
                        item.delegate = strategiesDelegate
                    }
                }

                ListModel {
                    id: strategiesModel

                    ListElement {
                        text: "Option 1"
                    }

                    ListElement {
                        text: "Option 2"
                    }

                    ListElement {
                        text: "Option 3"
                    }

                    ListElement {
                        text: "Option 4"
                    }
                }

                Component {
                    id: strategiesHeader

                    ComboBox {
                        width: 120
                        model: ["click and step", "hover and step", "click and jump", "click and spin"]
                    }
                }

                Component {
                    id: strategiesDelegate

                    Button {
                        text: model.text
                        width: 100

                        SpinBox {
                            width: 75
                            decimals: 1
                            minimumValue: 0
                            stepSize: 0.1
                            visible: true
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

                Loader {
                    id: fourthPanel
                    source: "///qml/CollapsiblePanel.qml"
                    anchors {
                        top: thirdPanel.bottom
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
                    id: fifthPanel
                    source: "///qml/CollapsiblePanel.qml"
                    anchors {
                        top: fourthPanel.bottom
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
