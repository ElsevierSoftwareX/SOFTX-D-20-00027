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
                //source: "///img/smaller001.png"
                fillMode: Image.PreserveAspectFit
                anchors.margins: 5
                anchors {
                    top: parent.top
                    bottom: slider.top
                    left: parent.left
                    right: parent.right
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        console.log(mouseX + " " + mouseY)
                        canvas.lastX = mouseX
                        canvas.lastY = mouseY
                        canvas.mouseAction = "leftClick"
                        canvas.requestPaint()
                    }
                    onPositionChanged: {
                        console.log(mouseX + " " + mouseY)
                        canvas.lastX = mouseX
                        canvas.lastY = mouseY
                        canvas.mouseAction = "hover"
                        canvas.requestPaint()
                    }
                }

                Item {
                    id: rectangles

                    Item {
                        property int posX: 100
                        property int posY: 185
                        property int rectWidth: 75
                        property int rectHeight: 85
                        property string rectColor: "empty"
                    }

                    Item {
                        property int posX: 185
                        property int posY: 100
                        property int rectWidth: 85
                        property int rectHeight: 75
                        property string rectColor: "empty"
                    }
                }

                Canvas {
                    id: canvas
                    anchors.fill: parent
                    onPaint: {
                        var context = getContext("2d")
                        context.lineWidth = 2
                        context.strokeStyle = "red"

                        for(var i = 0; i < rectangles.children.length; ++i) {
                            var rect = {
                                x: rectangles.children[i].posX,
                                y: rectangles.children[i].posY,
                                width: rectangles.children[i].rectWidth,
                                height: rectangles.children[i].rectHeight,
                                color: rectangles.children[i].rectColor,
                                contains: function(x, y) {
                                    return (this.x < x && x < this.x + this.width && this.y < y && y < this.y + this.height)
                                }
                            }
                            context.strokeRect(rect.x, rect.y, rect.width, rect.height)

                            if(rect.contains(lastX, lastY) && mouseAction == "leftClick")
                                rectangles.children[i].rectColor = "green"
                            else if(rect.contains(lastX, lastY) && rect.color !== "green" && mouseAction == "hover")
                                rectangles.children[i].rectColor = "yellow"
                            else if(rect.color !== "green")
                                rectangles.children[i].rectColor = "empty"

                            switch(rectangles.children[i].rectColor) {
                                case "empty":
                                    context.clearRect(rect.x, rect.y, rect.width, rect.height)
                                    break
                                case "green":
                                    context.fillStyle = "green"
                                    context.fillRect(rect.x, rect.y, rect.width, rect.height)
                                    break
                                case "yellow":
                                    context.fillStyle = "yellow"
                                    context.fillRect(rect.x, rect.y, rect.width, rect.height)
                            }
                        }

                        //context.beginPath()
                        //context.moveTo(100, 185)
                        //context.lineTo(175, 175)
                        //context.lineTo(200, 110)
                        //context.lineTo(100, 185)
                        ///context.stroke()
                    }

                    property real lastX: 0
                    property real lastY: 0
                    property string mouseAction
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
                anchors.rightMargin: 5
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: sliderValue.left
                }
                onValueChanged: {
                    //cellImage.source = ""
                    if(value < 10) cellImage.source = "///examples/example/smaller00%1.png".arg(value)
                    else if(value < 100) cellImage.source = "///examples/example/smaller0%1.png".arg(value)
                    else cellImage.source = "///examples/example/smaller%1.png".arg(value)
                    //cellImage.source = qsTr("image://celltracking/%1").arg("file:///Users/enrico/Documents/celltracker/example/smaller001.png")
                }
            }

            Text {
                id: sliderValue
                text: "%1/%2".arg(slider.value).arg(slider.maximumValue)
                font.pointSize: 14
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
                id: dialogLoader
            }

            Flickable {
                contentHeight: 540 * 2
                anchors.fill: parent

                /*ColumnLayout {
                    width: 300
                    anchors.margins: 10
                    anchors {
                        //top: parent.top
                        bottom: strategies.top
                        left: parent.left
                        right: parent.right
                    }*/

                Button {
                    id: firstButton
                    anchors {
                        top: parent.top
                        left: parent.left
                        right: parent.right
                    }
                    onClicked: {
                        if(cellOperations.visible == false) {
                            cellOperations.visible = true
                            secondButton.anchors.top = cellOperations.bottom
                        }
                        else {
                            cellOperations.visible = false
                            secondButton.anchors.top = firstButton.bottom
                        }
                    }
                }

                GroupBox {
                    id: cellOperations
                    title: "cell operations"
                    visible: false
                    anchors.margins: 10
                    anchors {
                        top: firstButton.bottom
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

                Button {
                    id: secondButton
                    anchors {
                        top: firstButton.bottom
                        left: parent.left
                        right: parent.right
                    }
                    onClicked: {
                        if(trackOperations.visible == false) {
                            trackOperations.visible = true
                            thirdButton.anchors.top = trackOperations.bottom
                        }
                        else {
                            trackOperations.visible = false
                            thirdButton.anchors.top = secondButton.bottom
                        }
                    }
                }

                GroupBox {
                    id: trackOperations
                    title: "track operations"
                    visible: false
                    anchors.margins: 10
                    anchors {
                        top: secondButton.bottom
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
                //}

                /*ColumnLayout {
                    id: strategies
                    height: children.height
                    anchors.margins: 10
                    anchors {
                        bottom: info.top
                        left: parent.left
                        right: parent.right
                    }*/

                Button {
                    id: thirdButton
                    anchors {
                        top: secondButton.bottom
                        left: parent.left
                        right: parent.right
                    }
                    onClicked: {
                        if(strategy.visible == false) {
                            strategy.visible = true
                            fourthButton.anchors.top = strategy.bottom
                        }
                        else {
                            strategy.visible = false
                            fourthButton.anchors.top = thirdButton.bottom
                        }
                    }
                }
                    GroupBox {
                        id: strategy
                        title: "strategies"
                        visible: false
                        anchors.margins: 10
                        anchors {
                            top: thirdButton.bottom
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
                //}

                /*ColumnLayout {
                    id: info
                    width: 150
                    anchors.margins: 10
                    anchors {
                        //top: strategies.bottom
                        bottom: parent.bottom
                        left: parent.left
                        right: parent.right
                    }*/


                    Button {
                        id: fourthButton
                        anchors {
                            top: thirdButton.bottom
                            left: parent.left
                            right: parent.right
                        }
                        onClicked: {
                            if(timeInfo.visible == false) {
                                timeInfo.visible = true
                                fifthButton.anchors.top = timeInfo.bottom
                            }
                            else {
                                timeInfo.visible = false
                                fifthButton.anchors.top = fourthButton.bottom
                            }
                        }
                    }


                    GroupBox {
                        id: timeInfo
                        title: "time info"
                        visible: false
                        anchors.margins: 10
                        anchors {
                            top: fourthButton.bottom
                            left: parent.left
                            right: parent.right
                        }

                        property int fontSize: 14

                        Column {

                            Text {
                                text: slider.value
                                font.pointSize: 28
                                width: 60

                                Text {
                                    text: "(of %1)".arg(slider.maximumValue)
                                    font.pointSize: timeInfo.fontSize
                                    anchors.bottom: parent.bottom
                                    anchors.left: parent.right
                                    anchors.leftMargin: 5
                                }
                            }

                            Text {
                                text: "# cells:"
                                font.pointSize: timeInfo.fontSize
                                width: 60

                                Text {
                                    text: "1"
                                    font.pointSize: timeInfo.fontSize
                                    anchors.left: parent.right
                                    anchors.leftMargin: 5
                                }
                            }
                        }
                    }

                    Button {
                        id: fifthButton
                        anchors {
                            top: fourthButton.bottom
                            left: parent.left
                            right: parent.right
                        }
                        onClicked: {
                            if(strategy.visible == false) {
                                cellInfo.visible = true
                                sixthButton.anchors.top = cellInfo.bottom
                            }
                            else {
                                cellInfo.visible = false
                                sixthButton.anchors.top = fifthButton.bottom
                            }
                        }
                    }

                    GroupBox {
                        id: cellInfo
                        title: "cell info"
                        visible: false
                        anchors.margins: 10
                        anchors {
                            top: fifthButton.bottom
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

                    Button {
                        id: sixthButton
                        anchors {
                            top: fifthButton.bottom
                            left: parent.left
                            right: parent.right
                        }
                        onClicked: {
                            if(trackInfo.visible == false) {
                                trackInfo.visible = true
                                //fourthButton.anchors.top = trackInfo.bottom
                            }
                            else {
                                trackInfo.visible = false
                                //fourthButton.anchors.top = thirdButton.bottom
                            }
                        }
                    }

                    GroupBox {
                        id: trackInfo
                        title: "current track info"
                        visible: false
                        anchors.margins: 10
                        anchors {
                            top: sixthButton.bottom
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
                            font.pointSize: timeInfo.fontSize
                            width: 90

                            Text {
                                text: model.value
                                font.pointSize: timeInfo.fontSize
                                anchors.left: parent.right
                                anchors.leftMargin: 5
                            }
                        }
                    }
                //}
            }
        }
    }
}
