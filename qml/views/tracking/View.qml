import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.1

Item {

    RowLayout {
        height: window.height * 0.9
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
                source: "///img/smaller001.png"
                fillMode: Image.PreserveAspectFit
                anchors.margins: 5
                anchors {
                    top: parent.top
                    bottom: slider.top
                    left: parent.left
                    right: parent.right
                }
            }

            Slider {
                id: slider
                objectName: "horizontalSlider"
                minimumValue: 1
                maximumValue: 209
                value: 1
                stepSize: 1
                updateValueWhileDragging: true
                orientation: Qt.Horizontal
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }
                onValueChanged: {
                    //cellImage.source = ""
                    if(value < 10) cellImage.source = "///examples/example/smaller00%1.png".arg(value)
                    else if(value < 100) cellImage.source = "///examples/example/smaller0%1.png".arg(value)
                    else cellImage.source = "///examples/example/smaller%1.png".arg(value)
                    //cellImage.source = qsTr("image://celltracking/%1").arg(cellImage.source)
                }
            }
        }

        Rectangle {
            id: sidebar
            width: sidebarIsExpanded ? 480 : 0
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
                contentHeight: 540
                anchors.fill: parent

                ColumnLayout {
                    width: 300
                    anchors.margins: 10
                    anchors {
                        top: parent.top
                        bottom: strategies.top
                        left: parent.left
                    }

                    GroupBox {
                        title: "cell operations"
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        Column {
                            spacing: 5

                            Button {
                                text: "add cell"
                                width: 100

                                CheckBox {
                                    text: "auto"
                                    checked: true
                                    anchors.left: parent.right
                                    anchors.leftMargin: 5
                                }
                            }

                            Button {
                                text: "remove cell"
                                width: 100
                            }

                            Button {
                                text: "remove all cells till now"
                                width: 180
                                onClicked: {
                                    dialogLoader.sourceComponent = undefined
                                    dialogLoader.sourceComponent = removeTillNowDialog
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
                            }

                            Button {
                                text: "remove all cells from now"
                                width: 180
                                onClicked: {
                                    dialogLoader.sourceComponent = undefined
                                    dialogLoader.sourceComponent = removeFromNowDialog
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

                    GroupBox {
                        title: "track operations"
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        Column {
                            spacing: 5

                            Text {
                                text: "current track:"
                                font.pointSize: 16
                                height: 25
                                width: 160

                                ComboBox {
                                    width: 120
                                    model: ["0"]
                                    anchors.left: parent.right
                                    anchors.leftMargin: 5
                                }
                            }

                            Button {
                                text: "create new track"
                                width: 160
                            }

                            Button {
                                text: "remove current track"
                                width: 160
                                onClicked: {
                                    dialogLoader.sourceComponent = undefined
                                    dialogLoader.sourceComponent = removeCurrentTrackDialog
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
                            }

                            Item {
                                Layout.fillHeight: true
                            }

                            Button {
                                text: "change tracks"
                                width: 160

                                ComboBox {
                                    width: 120
                                    model: ["", "0 (is current track)"]
                                    anchors.left: parent.right
                                    anchors.leftMargin: 5
                                }
                            }

                            Button {
                                text: "add daughter track"
                                width: 160

                                ComboBox {
                                    width: 120
                                    model: ["", "0 (is current track)"]
                                    anchors.left: parent.right
                                    anchors.leftMargin: 5
                                }
                            }

                            Button {
                                text: "remove daughter tracks"
                                width: 160
                            }

                            Text {
                                text: "change track status:"
                                font.pointSize: 16
                                height: 25
                                width: 160

                                ComboBox {
                                    width: 120
                                    model: ["open", "cell division", "dead", "lost", "end of movie reached"]
                                    anchors.left: parent.right
                                    anchors.leftMargin: 5
                                }
                            }
                        }
                    }
                }

                ColumnLayout {
                    id: strategies
                    height: children.height
                    anchors.margins: 10
                    anchors {
                        bottom: parent.bottom
                        left: parent.left
                        right: parent.right
                    }

                    GroupBox {
                        id: strategy
                        title: "strategies"
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        property int isChosen: 0

                        Column {
                            spacing: 5

                            Button {
                                text: "click and step"
                                width: 100
                                onClicked: {
                                    strategy.isChosen = strategy.isChosen == 1 ? 0 : 1
                                }

                                Button {
                                    text: "hover and step"
                                    width: 100
                                    anchors.left: parent.right
                                    anchors.leftMargin: 5
                                    onClicked: {
                                        strategy.isChosen = strategy.isChosen == 2 ? 0 : 2
                                    }

                                    SpinBox {
                                        width: 75
                                        decimals: 1
                                        minimumValue: 0
                                        stepSize: 0.1
                                        visible: strategy.isChosen == 2 ? true : false
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

                            Button {
                                text: "click and jump"
                                width: 100
                                onClicked: {
                                    strategy.isChosen = strategy.isChosen == 3 ? 0 : 3
                                }

                                Button {
                                    text: "click and spin"
                                    width: 100
                                    anchors.left: parent.right
                                    anchors.leftMargin: 5
                                    onClicked: {
                                        strategy.isChosen = strategy.isChosen == 4 ? 0 : 4
                                    }

                                    SpinBox {
                                        width: 75
                                        decimals: 1
                                        minimumValue: 0.2
                                        stepSize: 0.1
                                        visible: strategy.isChosen == 4 ? true : false
                                        anchors.left: parent.right
                                        anchors.leftMargin: 5
                                    }
                                }
                            }
                        }
                    }
                }

                ColumnLayout {
                    id: info
                    width: 150
                    anchors.margins: 10
                    anchors {
                        top: parent.top
                        bottom: strategies.top
                        right: parent.right
                    }

                    property int fontSize: 14

                    GroupBox {
                        title: "time info"
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        Column {

                            Text {
                                text: slider.value
                                font.pointSize: 28
                                width: 60

                                Text {
                                    text: "(of %1)".arg(slider.maximumValue)
                                    font.pointSize: info.fontSize
                                    anchors.bottom: parent.bottom
                                    anchors.left: parent.right
                                    anchors.leftMargin: 5
                                }
                            }

                            Text {
                                text: "# cells:"
                                font.pointSize: info.fontSize
                                width: 60

                                Text {
                                    text: "1"
                                    font.pointSize: info.fontSize
                                    anchors.left: parent.right
                                    anchors.leftMargin: 5
                                }
                            }
                        }
                    }

                    GroupBox {
                        title: "cell info"
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        ListView {
                            height: count > 0 ? contentHeight : 0
                            delegate: textDelegate

                            model: ListModel {

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
                        }
                    }

                    GroupBox {
                        title: "current track info"
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        ListView {
                            height: count > 0 ? contentHeight : 0
                            delegate: textDelegate

                            model: ListModel {

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
                        }
                    }

                    Component {
                        id: textDelegate

                        Text {
                            text: model.text
                            font.pointSize: info.fontSize
                            width: 90

                            Text {
                                text: model.value
                                font.pointSize: info.fontSize
                                anchors.left: parent.right
                                anchors.leftMargin: 5
                            }
                        }
                    }
                }
            }
        }
    }
}
