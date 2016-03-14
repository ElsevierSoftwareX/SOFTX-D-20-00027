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

    function viewActivationHook() {
        GUIController.changeFrameAbs(GUIState.currentFrame)
        cellImage.updateImage()
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

                        property int cut1X: 0
                        property int cut1Y: 0
                        onPressed: {
                            updateMousePosition()
                            cut1X = GUIState.mouseX
                            cut1Y = GUIState.mouseY
                        }

                        onReleased: {
                            updateMousePosition()
                            var cut2X = GUIState.mouseX
                            var cut2Y = GUIState.mouseY
                            GUIController.cutObject(cut1X, cut1Y, cut2X, cut2Y)
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

            Flickable {
                /* This is a flickable element that arranges the collapsible panels
                   in the sidebar. Each panel needs a model for showing information
                   and a delegate to implement the functionality. */
                contentHeight: 0
                anchors.fill: parent
                anchors.leftMargin: 5
                id: flick
            }
        }
    }
}
