import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1
import imb.celltracker 1.0
import "."

Item {
    /* This is the element for showing the main window of the tracking
       view. It consists of a workspace area and a sidebar. */
    property string mode: "sep"

    function viewActivationHook() {
        GUIController.changeFrameAbs(GUIState.currentFrame)
        resetOutlineVariables()
        cellImage.updateImage()
    }

    function viewDeactivationHook() {
        resetOutlineVariables()
    }

    function resetOutlineVariables() {
        GUIState.drawCutLine = false
        GUIState.drawAggregation = false
        GUIState.drawSeparation = false
        GUIState.drawDeletion = false
        GUIState.drawFlood = false
        GUIState.startX = -1
        GUIState.startY = -1
        GUIState.endX = -1
        GUIState.endY = -1
    }

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

            Rectangle {
                clip: true
                anchors {
                    top: parent.top
                    bottom: slider.top
                    left: parent.left
                    right: parent.right
                }

                Image {
                    /* This is the image element for drawing the frames. It contains
                     * properties that shall be shown in the sidebar. The mouse area
                     * triggers an event to reload the image provider, when the user
                     * has moved the cursor. */
                    id: cellImage
                    cache: false
                    fillMode: Image.PreserveAspectFit
                    anchors.fill: parent
                    anchors.margins: 5
                    sourceSize.width: width
                    sourceSize.height: height

                    onStatusChanged: GUIState.imageReady = status === Image.Ready

                    transform: [
                        Scale {
                            id: imgScale
                            xScale: GUIState.zoomFactor
                            yScale: GUIState.zoomFactor
                        },
                        Translate {
                            id: imgTranslate
                            x: GUIState.offX
                            y: GUIState.offY
                        }
                    ]

                    function updateImage() {
                        cellImage.source = "";
                        cellImage.source = "image://celltracking/";
                    }

                    Connections {
                        target: MessageRelay
                        onFinishNotification: cellImage.updateImage()
                    }

                    Connections {
                        target: GUIState
                        onCurrentFrameChanged: cellImage.updateImage()
                        onSelectedCellIDChanged: cellImage.updateImage()
                        onHoveredCellIDChanged: cellImage.updateImage()
                        onSelectedTrackIDChanged: cellImage.updateImage()
                        onHoveredTrackIDChanged: cellImage.updateImage()
                        onSelectedAutoTrackIDChanged: cellImage.updateImage()
                        onHoveredAutoTrackIDChanged: cellImage.updateImage()
                        onBackingDataChanged: cellImage.updateImage()
                        onZoomFactorChanged: cellImage.updateImage()
                        onMouseXChanged: cellImage.updateImage()
                        onMouseYChanged: cellImage.updateImage()
                        onDrawCutLineChanged: cellImage.updateImage()
                        onDrawAggregationChanged: cellImage.updateImage()
                        onDrawSeparationChanged: cellImage.updateImage()
                        onDrawDeletionChanged: cellImage.updateImage()
                        onDrawFloodChanged: cellImage.updateImage()
                        onStartXChanged: cellImage.updateImage()
                        onStartYChanged: cellImage.updateImage()
                        onEndXChanged: cellImage.updateImage()
                        onEndYChanged: cellImage.updateImage()
                        onThreshChanged: cellImage.updateImage()
                    }

                    property real offsetWidth: (width - paintedWidth) / 2
                    property real offsetHeight: (height - paintedHeight) / 2

                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        enabled: GUIState.mouseAreaActive
                        acceptedButtons: Qt.LeftButton | Qt.RightButton

                        hoverEnabled: true

                        function updateMousePosition() {
                            GUIState.mouseX = (mouseX - parent.offsetWidth)
                            GUIState.mouseY = (mouseY - parent.offsetHeight)
                        }

                        onPressed: {
                            if (mode === "sep") {
                                updateMousePosition()
                                GUIState.startX = GUIState.mouseX
                                GUIState.startY = GUIState.mouseY
                                GUIState.endX = -1
                                GUIState.endY = -1
                                GUIState.drawCutLine = true
                                GUIState.drawSeparation = false
                            }
                        }

                        onPositionChanged: {
                            if (GUIState.drawCutLine && mode === "sep")
                                updateMousePosition()
                        }

                        onReleased: {
                            if (mode === "sep") {
                                updateMousePosition()
                                GUIState.endX = GUIState.mouseX
                                GUIState.endY = GUIState.mouseY
                                GUIState.drawSeparation = true
                            }
                        }

                        onWheel: {
                            updateMousePosition();
                            var diff = (wheel.angleDelta.y > 0) ? 1 : -1
                            GUIState.thresh += diff
                        }

                        onClicked: {
                            if (mode === "agg") {
                                updateMousePosition()
                                if (GUIState.startX === -1 && GUIState.startY === -1) { /* no point selected */
                                    GUIState.startX = GUIState.mouseX
                                    GUIState.startY = GUIState.mouseY
                                    GUIState.drawAggregation = true
                                } else if (GUIState.endX === -1 && GUIState.endY === -1) { /* one point already selected */
                                    GUIState.endX = GUIState.mouseX
                                    GUIState.endY = GUIState.mouseY
                                    GUIState.drawAggregation = true
                                } else { /* both points selected, but clicked again, so reset second point */
                                    GUIState.startX = GUIState.mouseX
                                    GUIState.startY = GUIState.mouseY
                                    GUIState.endX = -1
                                    GUIState.endY = -1
                                }
                            }
                            if (mode === "del") {
                                updateMousePosition()
                                GUIState.startX = GUIState.mouseX
                                GUIState.startY = GUIState.mouseY
                                GUIState.drawDeletion = true
                            }
                            if (mode === "ff") {
                                updateMousePosition()
                                GUIState.thresh = 5
                                GUIState.startX = GUIState.mouseX
                                GUIState.startY = GUIState.mouseY
                                GUIState.drawFlood = true
                            }
                        }

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
                                updateMousePosition()
                                if (mode === "sep") {
                                    GUIController.cutObject(GUIState.startX, GUIState.startY, GUIState.endX, GUIState.endY)
                                } else if (mode === "agg") {
                                    GUIController.mergeObjects(GUIState.startX, GUIState.startY, GUIState.endX, GUIState.endY)
                                } else if (mode === "del") {
                                    GUIController.deleteObject(GUIState.startX, GUIState.startY)
                                } else if (mode === "ff") {
                                    GUIController.floodFill(GUIState.startX, GUIState.startY)
                                }

                                resetOutlineVariables()
                                break;
                            }
                        }

                        focus: true
                    }
                }
            }

            Slider {
                /* This is the slider element for navigating through the frames.
                   Reloads the image provider and updates the properties, if its
                   value or the mouse position has changed. */
                id: slider
                minimumValue: 0
                maximumValue: GUIState.maximumFrame
                value: 0
                stepSize: 1
                updateValueWhileDragging: true
                orientation: Qt.Horizontal
                anchors.rightMargin: 5
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: currentFrameDisplay.left
                }

                onValueChanged: GUIController.changeFrameAbs(value)

                Component.onCompleted: GUIState.setSlider(slider)
            }

            Text {
                id: currentFrameDisplay
                text: "%1/%2".arg(slider.value).arg(slider.maximumValue)
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
            width: sidebarIsExpanded ? 300 : 0
            color: "#dddddd"
            visible: sidebarIsExpanded ? true : false
            anchors {
                top: parent.top
                bottom: parent.bottom
                right: parent.right
            }
            Text
            {
                id: warning
                anchors.top: parent.top
                width: parent.width
                text: "Warning! Every change done here can't be reversed (changes are immediately saved to the project file)!"
                color: "red"
                font.pixelSize: 14
                wrapMode: Text.WordWrap
            }
            ColumnLayout {
                anchors.top: warning.bottom
                Button {
                    text: "Separation"
                    onClicked: {
                        resetOutlineVariables()
                        mode = "sep"
                    }
                    style: ButtonStyle {
                        label: Text {
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            font.pixelSize: 12
                            color: (parent.enabled)? ((mode === "sep")? "red" : "black") : "gray"
                            text: control.text
                        }
                    }
                }

                Button {
                    text: "Aggregation"
                    onClicked: {
                        resetOutlineVariables()
                        mode = "agg"
                    }
                    style: ButtonStyle {
                        label: Text {
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            font.pixelSize: 12
                            color: (parent.enabled)? ((mode === "agg")? "red" : "black") : "gray"
                            text: control.text
                        }
                    }
                }

                Button {
                    text: "Delete Cell"
                    onClicked: {
                        resetOutlineVariables()
                        mode = "del"
                    }
                    style: ButtonStyle {
                        label: Text {
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            font.pixelSize: 12
                            color: (parent.enabled)? ((mode === "del")? "red" : "black") : "gray"
                            text: control.text
                        }
                    }
                }

                Button {
                    text: "Add Cell (FloodFill) (thresh %1)".arg(GUIState.thresh)
                    onClicked: {
                        resetOutlineVariables()
                        mode = "ff"
                    }
                    style: ButtonStyle {
                        label: Text {
                            verticalAlignment: Text.AlignVCenter
                            horizontalAlignment: Text.AlignHCenter
                            font.pixelSize: 12
                            color: (parent.enabled)? ((mode === "ff")? "red" : "black") : "gray"
                            text: control.text
                        }
                    }
                }
            }
        }
    }
}
