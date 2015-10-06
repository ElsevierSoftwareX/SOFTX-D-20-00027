import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Controls 1.3
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import imb.celltracker 1.0
import "."

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
                   properties that shall be shown in the sidebar. The mouse area
                   triggers an event to reload the image provider, when the user
                   has moved the cursor. */
                    id: cellImage
                    cache: false
                    fillMode: Image.PreserveAspectFit
                    anchors.fill: parent
                    anchors.margins: 5
                    sourceSize.width: width
                    sourceSize.height: height

                    transform: [
                        Scale {
                            origin.x: cellImage.width/2
                            origin.y: cellImage.height/2
                            xScale: GUIState.zoomFactor
                            yScale: GUIState.zoomFactor
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

                        onClicked: {
                            updateMousePosition();
                            GUIController.selectCell(GUIState.currentFrame, GUIState.mouseX, GUIState.mouseY);
                            if (mouse.button == Qt.RightButton)
                                contextMenu.popup()
                        }

                        onPositionChanged: {
                            updateMousePosition();
                            GUIController.hoverCell(GUIState.currentFrame, GUIState.mouseX, GUIState.mouseY)
                        }
                        onWheel: {
                            if (wheel.modifiers & Qt.ControlModifier)
                                GUIState.zoomFactor += (wheel.angleDelta.y > 0)?(0.05):(-0.05)
                            else
                                GUIController.changeFrame((wheel.angleDelta.y > 0)?(+1):(-1))
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
                                    updateMousePosition();
                                    if (GUIController.currentStrategy == GUIState.STRATEGY_DEFAULT) {
                                        var ret = GUIController.connectTracks();
                                        if (ret) {
                                            GUIController.selectCell(GUIState.currentFrame, GUIState.mouseX, GUIState.mouseY);
                                            slider.value += 1;
                                        }
                                    } else {
                                        GUIController.abortStrategy();
                                    }

                                    break;
                                case GUIState.ACTION_ADD_DAUGHTERS:
                                    GUIController.setCurrentAction(GUIState.ACTION_DEFAULT);
                                    break;
                                }
                                break;
                            }
                        }

                        CTContextMenu {
                            id: contextMenu
                        }

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

            Flickable {
                /* This is a flickable element that arranges the collapsible panels
                   in the sidebar. Each panel needs a model for showing information
                   and a delegate to implement the functionality. */
                contentHeight: cellInfo.height + selectedInfo.height + navigationPanel.height + actionsPanel.height + strategiesPanel.height
                anchors.fill: parent
                id: flick

                /* ================= text-value delegate that is used for some of the panels ================= */
                Component {
                    id: textValueDelegate

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

                /* ================= Panel cellInfo ================= */
                property list<QtObject> cellInfoModel: [
                    QtObject { property string text: "cell ID"; property int value: GUIState.hoveredCellID },
                    QtObject { property string text: "frame ID"; property int value: GUIState.currentFrame },
                    QtObject { property string text: "autoTracklet ID"; property int value: GUIState.hoveredAutoTrackID },
                    QtObject { property string text: "autoTracklet start"; property int value: GUIState.hoveredAutoTrackStart },
                    QtObject { property string text: "autoTracklet end"; property int value: GUIState.hoveredAutoTrackEnd },
                    QtObject { property string text: "autoTracklet length"; property int value: GUIState.hoveredAutoTrackLength },
                    QtObject { property string text: "tracklet ID"; property int value: GUIState.hoveredTrackID },
                    QtObject { property string text: "tracklet start"; property int value: GUIState.hoveredTrackStart },
                    QtObject { property string text: "tracklet end"; property int value: GUIState.hoveredTrackEnd },
                    QtObject { property string text: "tracklet length"; property int value: GUIState.hoveredTrackLength }
                ]

                CTCollapsiblePanel {
                    id: cellInfo
                    anchors { top: parent.top; left: parent.left; right: parent.right }
                    titleText: "hovered info"
                    state: "expanded"
                    model: flick.cellInfoModel
                    delegate: textValueDelegate
                }

                /* ================= Panel selectedInfo ================= */
                property list<QtObject> selectedCellModel: [
                    QtObject { property string text: "cell ID"; property int value: GUIState.selectedCellID },
                    QtObject { property string text: "frame ID"; property int value: GUIState.currentFrame },
                    QtObject { property string text: "autoTracklet ID"; property int value: GUIState.selectedAutoTrackID },
                    QtObject { property string text: "autoTracklet start"; property int value: GUIState.selectedAutoTrackStart },
                    QtObject { property string text: "autoTracklet end"; property int value: GUIState.selectedAutoTrackEnd },
                    QtObject { property string text: "autoTracklet length"; property int value: GUIState.selectedAutoTrackLength },
                    QtObject { property string text: "tracklet ID"; property int value: GUIState.selectedTrackID },
                    QtObject { property string text: "tracklet start"; property int value: GUIState.selectedTrackStart },
                    QtObject { property string text: "tracklet end"; property int value: GUIState.selectedTrackEnd },
                    QtObject { property string text: "tracklet length"; property int value: GUIState.selectedTrackLength }
                ]

                CTCollapsiblePanel {
                    id: selectedInfo
                    anchors { top: cellInfo.bottom; left: parent.left; right: parent.right }
                    titleText: "selected info"
                    state: "expanded"
                    model: flick.selectedCellModel
                    delegate: textValueDelegate
                }

                /* ================= Panel navigationsPanel ================= */
                property list<QtObject> navigationModel: [
                    QtObject { property string text: "start of track"; property int targetFrame: GUIState.selectedTrackStart },
                    QtObject { property string text: "end of track"; property int targetFrame: GUIState.selectedTrackEnd },
                    QtObject { property string text: "start of autotrack"; property int targetFrame: GUIState.selectedAutoTrackStart },
                    QtObject { property string text: "end of autotrack"; property int targetFrame: GUIState.selectedAutoTrackEnd }
                ]

                CTCollapsiblePanel {
                    id: navigationPanel
                    anchors { top: selectedInfo.bottom; left: parent.left; right: parent.right }
                    titleText: "navigation"
                    state: "expanded"
                    model: flick.navigationModel
                    delegate: Button {
                        id: navigationDelegate
                        text: model.text
                        width: 160
                        onClicked: {
                            if (model.targetFrame >= 0)
                                GUIController.changeFrameAbs(model.targetFrame)
                        }
                    }
                }

                /* ================= Panel actionsPanel ================= */
                property list<QtObject> actionsModel: [
                    QtObject { property string text: "add daughters"; property int val: GUIState.ACTION_ADD_DAUGHTERS },
                    QtObject { property string text: "delete cell"; property int val: GUIState.ACTION_DELETE_CELL },
                    QtObject { property string text: "delete all from cell on"; property int val: GUIState.ACTION_DELETE_CELLS_FROM },
                    QtObject { property string text: "delete all until cell"; property int val: GUIState.ACTION_DELETE_CELLS_TILL }
                ]

                CTCollapsiblePanel {
                    id: actionsPanel
                    anchors { top: navigationPanel.bottom; left: parent.left; right: parent.right }
                    titleText : "actions"
                    state : "expanded"
                    model : flick.actionsModel
                    delegate : actionsDelegate
                }

                Component {
                    id: actionsDelegate

                    Button {
                        text: model.text
                        width: 160
                        onClicked: {
                            /* toggle between action */
                            if (GUIController.currentAction === model.val)
                                GUIController.setCurrentAction(GUIState.ACTION_DEFAULT);
                            else
                                GUIController.setCurrentAction(model.val);
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
                    }
                }

                /* ================= Panel strategiesPanel ================= */
                property list<QtObject> strategiesModel: [
                    QtObject { property string text: "click & jump"; property int val: GUIState.STRATEGY_CLICK_JUMP; property bool skip: true; property bool delay: true; },
                    QtObject { property string text: "click & spin"; property int val: GUIState.STRATEGY_CLICK_SPIN; property bool skip: false; property bool delay: true; },
                    QtObject { property string text: "click & step"; property int val: GUIState.STRATEGY_CLICK_STEP; property bool skip: false; property bool delay: true; },
                    QtObject { property string text: "hover & step"; property int val: GUIState.STRATEGY_HOVER_STEP; property bool skip: false; property bool delay: true; }
                ]

                CTCollapsiblePanel {
                    id: strategiesPanel
                    anchors { top: actionsPanel.bottom; left: parent.left; right: parent.right }
                    titleText : "strategies"
                    state : "expanded"
                    model : flick.strategiesModel
                    delegate : strategiesDelegate
                }

                Component {
                    id: strategiesDelegate

                    RowLayout {
                        Button {
                            id: strategyButton
                            text: model.text
                            enabled: (GUIController.currentStrategy === GUIState.STRATEGY_DEFAULT || GUIController.currentStrategy === model.val)
                            Layout.minimumWidth: 160
                            Layout.maximumWidth: 160
                            onClicked: {
                                if (GUIController.currentStrategy === model.val)
                                    GUIController.currentStrategy = GUIState.STRATEGY_DEFAULT;
                                else
                                    GUIController.currentStrategy = model.val;

                                if (GUIController.currentStrategyRunning) {
                                    GUIController.abortStrategy();
                                } else {
                                    GUIController.startStrategy(delayVal.text, showVal.text);
                                }
                            }

                            style: ButtonStyle {
                                label: Text {
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                    font.pixelSize: 12
                                    color: (parent.enabled)?
                                               ((GUIController.currentStrategyRunning)?
                                                    "red" :
                                                    "black") :
                                               "gray"
                                    text: control.text
                                }
                            }

                        }

                        TextField {
                            id: delayVal
                            text: CTSettings.value("strategies/delay_val")
                            visible: model.delay
                            Layout.minimumWidth: 60
                            Layout.maximumWidth: 60

                            validator: IntValidator {
                                bottom: 1
                                top: 60000
                            }
                        }

                        TextField {
                            id: showVal
                            text: CTSettings.value("strategies/show_val")
                            visible: model.skip
                            Layout.minimumWidth: 40
                            Layout.maximumWidth: 40

                            validator: IntValidator {
                                bottom: 1
                                top: GUIState.maximumFrame - GUIState.currentFrame
                            }
                        }
                    }
                }
            }
        }
    }
}
