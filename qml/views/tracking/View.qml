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

                        onClicked: {
                            mouseArea.forceActiveFocus()
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
                            if (wheel.modifiers & Qt.ControlModifier) {
                                updateMousePosition();
                                var zoomDiff = (wheel.angleDelta.y > 0)? 1.05 : 1/1.05

                                if (GUIState.zoomFactor != (GUIState.zoomFactor *= zoomDiff)) {
                                    // only translate if zoomFactor actually changed
                                    GUIState.offX += (1-zoomDiff)* GUIState.mouseX * GUIState.zoomFactor
                                    GUIState.offY += (1-zoomDiff)* GUIState.mouseY * GUIState.zoomFactor
                                }
                            } else {
                                GUIController.changeFrame((wheel.angleDelta.y > 0)?(+1):(-1))
                            }
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
                                default:
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
                contentHeight: cellInfo.height + selectedInfo.height + navigationPanel.height + actionsPanel.height + strategiesPanel.height + eventPanel.height
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
                    QtObject { property string text: "cell ID"; property string value: GUIState.hoveredCellID + "\t" +
                                                                                    GUIState.selectedCellID },
                    QtObject { property string text: "frame ID"; property string value: GUIState.currentFrame + "\t" +
                                                                                     GUIState.selectedCellFrame },
                    QtObject { property string text: "autoTracklet ID"; property string value: GUIState.hoveredAutoTrackID + "\t" +
                                                                                            GUIState.selectedAutoTrackID },
                    QtObject { property string text: "autoTracklet start"; property string value: GUIState.hoveredAutoTrackStart + "\t" +
                                                                                               GUIState.selectedAutoTrackStart },
                    QtObject { property string text: "autoTracklet end"; property string value: GUIState.hoveredAutoTrackEnd + "\t" +
                                                                                             GUIState.selectedAutoTrackEnd },
                    QtObject { property string text: "autoTracklet length"; property string value: GUIState.hoveredAutoTrackLength + "\t" +
                                                                                                GUIState.selectedAutoTrackLength },
                    QtObject { property string text: "tracklet ID"; property string value: GUIState.hoveredTrackID + "\t" +
                                                                                        GUIState.selectedTrackID },
                    QtObject { property string text: "tracklet start"; property string value: GUIState.hoveredTrackStart + "\t" +
                                                                                           GUIState.selectedTrackStart },
                    QtObject { property string text: "tracklet end"; property string value: GUIState.hoveredTrackEnd + "\t" +
                                                                                         GUIState.selectedTrackEnd },
                    QtObject { property string text: "tracklet length"; property string value: GUIState.hoveredTrackLength + "\t" +
                                                                                            GUIState.selectedTrackLength },
                    QtObject { property string text: "tracklet status"; property string value: GUIState.hoveredTrackStatus + "\t" +
                                                                                               GUIState.selectedTrackStatus },
                    QtObject { property string text: "tracklet mother"; property string value: GUIState.hoveredTrackMother + "\t" +
                                                                                               GUIState.selectedTrackMother },
                    QtObject { property string text: "tracklet daughters"; property string value: GUIState.hoveredTrackDaughters + "\t" +
                                                                                                  GUIState.selectedTrackDaughters }
                ]

                CTCollapsiblePanel {
                    id: cellInfo
                    anchors { top: parent.top; left: parent.left; right: parent.right }
                    titleText: "info hovered/selected"
                    state: "expanded"
                    model: flick.cellInfoModel
                    delegate: textValueDelegate
                }

                /* ================= Panel navigationsPanel ================= */
                property list<QtObject> navigationModel: [
                    QtObject { property string text: "start of track"; property int targetFrame: GUIState.selectedTrackStart; property bool enabled: GUIState.selectedTrackID !== -1 },
                    QtObject { property string text: "end of track"; property int targetFrame: GUIState.selectedTrackEnd; property bool enabled: GUIState.selectedTrackID !== -1 },
                    QtObject { property string text: "start of autotrack"; property int targetFrame: GUIState.selectedAutoTrackStart; property bool enabled: GUIState.selectedAutoTrackID !==  -1 },
                    QtObject { property string text: "end of autotrack"; property int targetFrame: GUIState.selectedAutoTrackEnd; property bool enabled: GUIState.selectedAutoTrackID !== -1 },
                    QtObject { property string text: "selected cell"; property int targetFrame: GUIState.selectedCellFrame; property bool enabled: GUIState.selectedCellID !== -1 }
                ]

                CTCollapsiblePanel {
                    id: navigationPanel
                    anchors { top: cellInfo.bottom; left: parent.left; right: parent.right }
                    titleText: "navigation"
                    state: "expanded"
                    model: flick.navigationModel
                    delegate: Button {
                        id: navigationDelegate
                        text: model.text
                        enabled: model.enabled
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
                    QtObject { property string text: "click & step"; property int val: GUIState.STRATEGY_CLICK_STEP; property bool skip: false; property bool delay: true; }
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
                                if (GUIController.currentStrategy === model.val) {
                                    GUIController.currentStrategy = GUIState.STRATEGY_DEFAULT
                                } else {
                                    GUIController.currentStrategy = model.val
                                }

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

                /* ================= Panel eventPanel ================= */
                /* Attention! Qt does not allow template classes to be registered with QML. So we cannot register
                 * the enum EVENT_TYPE with QML. So we have to assign values explicitly in C++ as well as here… */
                property list<QtObject> eventModel: [
                    QtObject { property string text: "open";         property int type: -1 /* open */;                property bool enabled: true},
                    QtObject { property string text: "division";     property int type: 0  /* EVENT_TYPE_DIVISION */; property bool enabled: false},
                    QtObject { property string text: "merge";        property int type: 1  /* EVENT_TYPE_MERGE */;    property bool enabled: false},
                    QtObject { property string text: "unmerge";      property int type: 2  /* EVENT_TYPE_UNMERGE */;  property bool enabled: false},
                    QtObject { property string text: "lost";         property int type: 3  /* EVENT_TYPE_LOST */;     property bool enabled: true},
                    QtObject { property string text: "death";        property int type: 4  /* EVENT_TYPE_DEAD */;     property bool enabled: true},
                    QtObject { property string text: "end of movie"; property int type: -2 /* unimplemented */;       property bool enabled: false}
                ]

                CTCollapsiblePanel {
                    id: eventPanel
                    anchors { top: strategiesPanel.bottom; left: parent.left; right: parent.right }
                    titleText: "events"
                    state: "expanded"
                    model: flick.eventModel
                    delegate: eventsDelegate
                }

                Component {
                    id: eventsDelegate
                    RowLayout {
                        id: eventButton
                        Button {
                            text: "set tracklet to " + model.text
                            enabled: model.enabled
                            Layout.minimumWidth: 200
                            Layout.maximumWidth: 200
                            onClicked: GUIController.changeStatus(GUIState.selectedTrackID, type)
                        }
                    }
                }
            }
        }
    }
}
