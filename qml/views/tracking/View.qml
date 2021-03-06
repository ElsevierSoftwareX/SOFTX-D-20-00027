/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2016, 2015 Enrico Uhlig, Konstantin Thierbach, Sebastian Wagner
 *
 * TraCurate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TraCurate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with TraCurate.  If not, see <https://www.gnu.org/licenses/>.
 */
import QtQuick 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1
import imb.tracurate 1.0
import "."

Item {
    /* This is the element for showing the main window of the tracking
       view. It consists of a workspace area and a sidebar. */

    function viewActivationHook() {
        GUIController.changeFrameAbs(GUIState.currentFrame)
        cellImage.updateImage()
    }

    function viewDeactivationHook() { }

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

                        onClicked: {
                            mouseArea.forceActiveFocus()
                            updateMousePosition();
                            GUIController.selectCell(GUIState.currentFrame,
                                                     GUIState.currentSlice,
                                                     GUIState.currentChannel,
                                                     GUIState.mouseX, GUIState.mouseY);
                            if (mouse.button == Qt.RightButton)
                                contextMenu.popup()
                        }

                        property int startDragX: 0
                        property int startDragY: 0
                        property bool dragActive: false
                        onPressed: {
                            updateMousePosition()
                            startDragX = GUIState.mouseX
                            startDragY = GUIState.mouseY
                            dragActive = true
                        }
                        onReleased: {
                            dragActive = false
                        }

                        onPositionChanged: {
                            updateMousePosition();
                            if (dragActive) {
                                GUIState.offX += GUIState.mouseX-startDragX
                                GUIState.offY += GUIState.mouseY-startDragY
                            }
                            GUIController.hoverCell(GUIState.currentFrame,
                                                    GUIState.currentSlice,
                                                    GUIState.currentChannel,
                                                    GUIState.mouseX, GUIState.mouseY)
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
                            case Qt.Key_W: /* start of tracklet */
                                if (GUIState.selectedTrackID !== -1)
                                    slider.value = GUIState.selectedTrackStart
                                break;
                            case Qt.Key_E: /* end of tracklet */
                                if (GUIState.selectedTrackID !== -1)
                                    slider.value = GUIState.selectedTrackEnd
                                break;
                            case Qt.Key_Q: /* start of autotracklet */
                                if (GUIState.selectedAutoTrackID !== -1)
                                    slider.value = GUIState.selectedAutoTrackStart
                                break;
                            case Qt.Key_R: /* end of autotracklet */
                                if (GUIState.selectedAutoTrackID !== -1)
                                    slider.value = GUIState.selectedAutoTrackEnd
                                break;
                            case Qt.Key_G:
                                GUIController.currentAction =
                                        (GUIController.currentAction === GUIState.ACTION_ADD_DAUGHTERS)?
                                            GUIState.ACTION_DEFAULT:
                                            GUIState.ACTION_ADD_DAUGHTERS
                                break;
                            case Qt.Key_H:
                                GUIController.currentAction =
                                        (GUIController.currentAction === GUIState.ACTION_ADD_MERGER)?
                                            GUIState.ACTION_DEFAULT:
                                            GUIState.ACTION_ADD_MERGER
                                break;
                            case Qt.Key_J:
                                 GUIController.currentAction =
                                        (GUIController.currentAction === GUIState.ACTION_ADD_UNMERGER)?
                                            GUIState.ACTION_DEFAULT:
                                            GUIState.ACTION_ADD_UNMERGER
                               break;
                            case Qt.Key_Z:
                                GUIController.currentAction =
                                        (GUIController.currentAction === GUIState.ACTION_DELETE_CELLS_FROM)?
                                            GUIState.ACTION_DEFAULT:
                                            GUIState.ACTION_DELETE_CELLS_FROM
                                break;
                            case Qt.Key_Left:
                                GUIState.offX -= TCSettings.value("scrolling/scroll_factor_x")*1
                                break;
                            case Qt.Key_Right:
                                GUIState.offX += TCSettings.value("scrolling/scroll_factor_x")*1
                                break;
                            case Qt.Key_Up:
                                GUIState.offY -= TCSettings.value("scrolling/scroll_factor_y")*1
                                break;
                            case Qt.Key_Down:
                                GUIState.offY += TCSettings.value("scrolling/scroll_factor_y")*1
                                break;
                            case Qt.Key_Space:
                                switch (GUIController.currentAction) {
                                case GUIState.ACTION_DEFAULT:
                                    updateMousePosition();
                                    if (GUIController.currentStrategy == GUIState.STRATEGY_DEFAULT) {
                                        var ret = GUIController.connectTracks();
                                        if (ret) {
                                            GUIController.selectCell(GUIState.currentFrame,
                                                                     GUIState.currentSlice,
                                                                     GUIState.currentChannel,
                                                                     GUIState.mouseX, GUIState.mouseY);
                                            slider.value += 1;
                                            if (GUIState.currentFrame == GUIState.maximumFrame)
                                                GUIController.changeStatus(GUIState.selectedTrackID, 5) /* end of movie */
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

                        TCContextMenu {
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

                style: SliderStyle { /* Basically the verbatim SliderStyle from Qt, but without the FastGlow (which causes problems on Windows) */
                    handle: Item {
                        implicitWidth:  implicitHeight
                        implicitHeight: TextSingleton.implicitHeight * 1.2

                        Rectangle {
                            id: handle
                            anchors.fill: parent
                            radius: width/2
                            gradient: Gradient { GradientStop { color: control.pressed ? "#e0e0e0" : "#fff"; position: 1 }
                                                 GradientStop { color: "#eee"; position: 0 } }
                            Rectangle {
                                anchors.fill: parent
                                anchors.margins: 1
                                radius: width/2
                                border.color: "#99ffffff"
                                color: control.activeFocus ? "#224f7fbf" : "transparent"
                            }
                            border.color: control.activeFocus ? "#47b" : "#777"
                        }
                    }
                }

                onValueChanged: GUIController.changeFrameAbs(value)

                Component.onCompleted: GUIState.setSlider(slider)
            }

            Text {
                id: currentFrameDisplay
                text: "%1/%2".arg(slider.value).arg(slider.maximumValue)
                color: "black"
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
                contentHeight: cellInfo.height + eventPanel.height +  navigationPanel.height + actionsPanel.height + strategiesPanel.height /* + slicesPanel.height + channelPanel.height */
                anchors.fill: parent
                anchors.leftMargin: 5
                id: flick

                /* ================= Panel cellInfo ================= */
                property list<QtObject> cellInfoModel: [
                    QtObject { property string desc: "cell ID";
                        property string hovered: GUIState.hoveredCellID;
                        property string selected: GUIState.selectedCellID },
                    QtObject { property string desc: "frame ID";
                        property string hovered: GUIState.hoveredCellFrame;
                        property string selected: GUIState.selectedCellFrame},
                    QtObject { property string desc: "autoTracklet ID";
                        property string hovered: GUIState.hoveredAutoTrackID;
                        property string selected: GUIState.selectedAutoTrackID },
                    QtObject { property string desc: "autoTracklet start";
                        property string hovered: GUIState.hoveredAutoTrackStart;
                        property string selected: GUIState.selectedAutoTrackStart },
                    QtObject { property string desc: "autoTracklet end";
                        property string hovered: GUIState.hoveredAutoTrackEnd;
                        property string selected: GUIState.selectedAutoTrackEnd },
                    QtObject { property string desc: "autoTracklet length";
                        property string hovered: GUIState.hoveredAutoTrackLength;
                        property string selected: GUIState.selectedAutoTrackLength },
                    QtObject { property string desc: "tracklet ID";
                        property string hovered: GUIState.hoveredTrackID;
                        property string selected: GUIState.selectedTrackID },
                    QtObject { property string desc: "tracklet start";
                        property string hovered: GUIState.hoveredTrackStart;
                        property string selected: GUIState.selectedTrackStart },
                    QtObject { property string desc: "tracklet end";
                        property string hovered: GUIState.hoveredTrackEnd;
                        property string selected: GUIState.selectedTrackEnd },
                    QtObject { property string desc: "tracklet length";
                        property string hovered: GUIState.hoveredTrackLength;
                        property string selected: GUIState.selectedTrackLength },
                    QtObject { property string desc: "tracklet status";
                        property string hovered: GUIState.hoveredTrackStatus;
                        property string selected: GUIState.selectedTrackStatus },
                    QtObject { property string desc: "previous tracklets";
                        property string hovered: GUIState.hoveredTrackPrevious;
                        property string selected: GUIState.selectedTrackPrevious },
                    QtObject { property string desc: "next tracklets";
                        property string hovered: GUIState.hoveredTrackNext;
                        property string selected: GUIState.selectedTrackNext }
                ]

                TCCollapsiblePanel {
                    id: cellInfo
                    anchors { top: parent.top; left: parent.left; right: parent.right }
                    titleText: "info"
                    state: "expanded"
                    model: flick.cellInfoModel
                    delegate: cellInfoDelegate
                    header: RowLayout {
                        spacing: 0
                        Rectangle { width: 120; height: 20; color: "transparent";
                            Text { text: "feature"; color: "black"; anchors.fill: parent; horizontalAlignment: Qt.AlignCenter } }
                        Rectangle { width:  70; height: 20; color: "transparent";
                            Text { text: "hovered"; color: "black"; anchors.fill: parent; horizontalAlignment: Qt.AlignCenter } }
                        Rectangle { width:  70; height: 20; color: "transparent";
                            Text { text: "selected"; color: "black"; anchors.fill: parent; horizontalAlignment: Qt.AlignCenter } }
                    }
                }

                Component {
                    id: cellInfoDelegate

                    Rectangle {
                        width: 260; height: 15;
                        color: (model.index%2 == 0)?Qt.rgba(0,0,0,0.25):Qt.rgba(0,0,0,0)
                        RowLayout {
                            width: 260
                            spacing: 0
                            Rectangle { width: 120; height: 15; color: "transparent";
                                Text { anchors.fill: parent; color: "black"; text: model.desc; horizontalAlignment: Qt.AlignCenter } }
                            Rectangle { width: 70; height: 15; color: "transparent";
                                Text { anchors.fill: parent; color: "black"; text: model.hovered === "-1"? "" : model.hovered; horizontalAlignment: Qt.AlignCenter } }
                            Rectangle { width: 70; height: 15; color: "transparent";
                                Text { anchors.fill: parent; color: "black"; text: model.selected === "-1"? "" : model.selected; horizontalAlignment: Qt.AlignCenter } }
                        }
                    }
                }

                /* ================= Panel eventPanel ================= */
                /* Attention! Qt does not allow template classes to be registered with QML. So we cannot register
                 * the enum EVENT_TYPE with QML. So we have to assign values explicitly in C++ as well as here… */
                property list<QtObject> eventModel: [
                    QtObject { property list<QtObject> items: [
                            QtObject { property string text: "open"; property int type: -1; /* open */ },
                            QtObject { property string text: "lost"; property int type: 3;  /* EVENT_TYPE_LOST */ },
                            QtObject { property string text: "dead"; property int type: 4;  /* EVENT_TYPE_DEAD */ },
                            QtObject { property string text: "end";  property int type: 5;  /* EVENT_TYPE_ENDOFMOVIE */ } ]}
                ]

                TCCollapsiblePanel {
                    id: eventPanel
                    anchors { top: cellInfo.bottom; left: parent.left; right: parent.right }
                    titleText: "set tracklet event"
                    state: "expanded"
                    model: flick.eventModel
                    delegate: eventsDelegate
                }

                Component {
                    id: eventsDelegate
                    RowLayout {
                        property var items: model.items
                        spacing: 5
                        Repeater {
                            model: items
                            Button {
                                text: model.text
                                implicitWidth: 50
                                onClicked: GUIController.changeStatus(GUIState.selectedTrackID, type)
                            }
                        }
                    }
                }

                /* ================= Panel navigationsPanel ================= */
                property list<QtObject> navigationModel: [
                    QtObject {
                        property string text: "Tracklet"
                        property list<QtObject> items: [
                            QtObject {
                                property string text: "start"
                                property int target: GUIState.selectedTrackStart;
                                property bool enabled: GUIState.selectedTrackID !== -1 },
                            QtObject {
                                property string text: "end"
                                property int target: GUIState.selectedTrackEnd;
                                property bool enabled: GUIState.selectedTrackID !== -1 }
                        ]},
                    QtObject {
                        property string text: "AutoTracklet"
                        property list<QtObject> items: [
                            QtObject {
                                property string text: "start"
                                property int target: GUIState.selectedAutoTrackStart;
                                property bool enabled: GUIState.selectedAutoTrackID !== -1 },
                            QtObject {
                                property string text: "end"
                                property int target: GUIState.selectedAutoTrackEnd;
                                property bool enabled: GUIState.selectedAutoTrackID !== -1 }
                        ]},
                    QtObject {
                        property string text: "Cell"
                        property list<QtObject> items: [
                            QtObject {
                                property string text: "jump to"
                                property int target: GUIState.selectedCellFrame;
                                property bool enabled: GUIState.selectedCellID !== -1 }
                        ]}
                ]

                Component {
                    id: navigationDelegate
                    RowLayout {
                        /* unfortunately I didn't find another way to do this */
                        property var items: model.items
                        Rectangle { width: 75; height: 20; color: "transparent"; Text { text: model.text; color: "black" } }
                        Repeater {
                            model: items
                            delegate: Button {
                                text: model.text
                                enabled: model.enabled
                                onClicked: if (model.target >= 0) GUIController.changeFrameAbs(model.target)
                            }
                        }
                    }
                }

                TCCollapsiblePanel {
                    id: navigationPanel
                    anchors { top: eventPanel.bottom; left: parent.left; right: parent.right }
                    titleText: "navigation"
                    state: "expanded"
                    model: flick.navigationModel
                    delegate: navigationDelegate
                }

                /* ================= Panel actionsPanel ================= */
                property list<QtObject> actionsModel: [
                    QtObject {
                        property string text: "add"
                        property list<QtObject> items: [
                            QtObject { property string text: "daughters"; property int action: GUIState.ACTION_ADD_DAUGHTERS },
                            QtObject { property string text: "merger";    property int action: GUIState.ACTION_ADD_MERGER },
                            QtObject { property string text: "unmerger";  property int action: GUIState.ACTION_ADD_UNMERGER }
                        ]},
                    QtObject {
                        property string text: "delete"
                        property list<QtObject> items: [
                            QtObject { property string text: "cell";           property int action: GUIState.ACTION_DELETE_CELL },
                            QtObject { property string text: "all until here"; property int action: GUIState.ACTION_DELETE_CELLS_TILL },
                            QtObject { property string text: "all from here";  property int action: GUIState.ACTION_DELETE_CELLS_FROM }
                        ]}
                ]

                Component {
                    id: actionsDelegate
                    RowLayout {
                        property var items: model.items
                        Rectangle { width: 50; height: 20; color: "transparent"; Text { text: model.text; color: "black" } }
                        GridLayout {
                            columns: 2
                            rowSpacing: 0
                            Repeater {
                                model: items
                                delegate: Button {
                                    text: model.text
                                    implicitHeight: 25
                                    implicitWidth: 100
                                    style: ButtonStyle {
                                        label: Text {
                                            text: control.text
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                            color: GUIController.currentAction === model.action ? "red" : "black"
                                        }
                                    }
                                    onClicked: GUIController.setCurrentAction(
                                                   GUIController.currentAction === model.action ? GUIState.ACTION_DEFAULT : model.action)
                                }
                            }
                        }
                    }
                }

                TCCollapsiblePanel {
                    id: actionsPanel
                    anchors { top: navigationPanel.bottom; left: parent.left; right: parent.right }
                    titleText : "actions"
                    state : "expanded"
                    model : flick.actionsModel
                    delegate : actionsDelegate
                }

                /* ================= Panel strategiesPanel ================= */
                property list<QtObject> strategiesModel: [
                    QtObject {
                        property string text: "click & jump";
                        property int val: GUIState.STRATEGY_CLICK_JUMP;
                        property bool skip: true;
                        property bool delay: true; },
                    /* QtObject {
                        property string text: "click & spin";
                        property int val: GUIState.STRATEGY_CLICK_SPIN;
                        property bool skip: false;
                        property bool delay: true; }, */
                    QtObject {
                        property string text: "click & step";
                        property int val: GUIState.STRATEGY_CLICK_STEP;
                        property bool skip: false;
                        property bool delay: true; }
                ]

                TCCollapsiblePanel {
                    id: strategiesPanel
                    anchors { top: actionsPanel.bottom; left: parent.left; right: parent.right }
                    titleText : "strategies"
                    state : "expanded"
                    model : flick.strategiesModel
                    delegate : strategiesDelegate
                    header: RowLayout {
                        Rectangle { width: 100; height: 20; color: "transparent";
                            Text { text: "";           color: "black"; anchors.fill: parent; horizontalAlignment: Qt.AlignCenter } }
                        Rectangle { width: 70;  height: 20; color: "transparent";
                            Text { text: "delay (ms)"; color: "black"; anchors.fill: parent; horizontalAlignment: Qt.AlignCenter } }
                        Rectangle { width: 70;  height: 20; color: "transparent";
                            Text { text: "# frames";   color: "black"; anchors.fill: parent; horizontalAlignment: Qt.AlignCenter } } }
                }

                Component {
                    id: strategiesDelegate

                    RowLayout {
                        Button {
                            id: strategyButton
                            text: model.text
                            enabled: (GUIController.currentStrategy === GUIState.STRATEGY_DEFAULT || GUIController.currentStrategy === model.val)
                            implicitWidth: 100
                            onClicked: {
                                GUIController.currentStrategy = (GUIController.currentStrategy === model.val)? GUIState.STRATEGY_DEFAULT : model.val

                                if (GUIController.currentStrategyRunning) GUIController.abortStrategy();
                                else GUIController.startStrategy(delayVal.text, showVal.text);
                            }

                            style: ButtonStyle {
                                label: Text {
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                    font.pixelSize: 12
                                    color: (parent.enabled)? ((GUIController.currentStrategyRunning)? "red" : "black") : "gray"
                                    text: control.text
                                }
                            }

                        }

                        TextField {
                            id: delayVal
                            text: TCSettings.value("strategies/delay_val")
                            visible: model.delay
                            enabled: !GUIController.currentStrategyRunning
                            implicitWidth: 70
                            horizontalAlignment: TextInput.AlignHCenter

                            validator: IntValidator {
                                bottom: 1
                                top: 60000
                            }

                            onAccepted: {
                                TCSettings.setValue("strategies/delay_val", delayVal.text)
                                mouseArea.forceActiveFocus()
                            }
                        }

                        TextField {
                            id: showVal
                            text: TCSettings.value("strategies/show_val")
                            visible: model.skip
                            enabled: !GUIController.currentStrategyRunning
                            implicitWidth: 70
                            horizontalAlignment: TextInput.AlignHCenter

                            validator: IntValidator {
                                bottom: 1
                                top: GUIState.maximumFrame - GUIState.currentFrame
                            }

                            onAccepted: {
                                TCSettings.setValue("strategies/show_val", showVal.text)
                                mouseArea.forceActiveFocus()
                            }
                        }
                    }
                }

                /* ================= Panel slicesPanel ================= */
/*                TCCollapsiblePanel {
                    id: slicesPanel
                    anchors { top: strategiesPanel.bottom; left: parent.left; right: parent.right }
                    titleText : "slices"
                    state : "expanded"
                    model : GUIState.maximumSlice
                    delegate : slicesDelegate
                }

                Component {
                    id: slicesDelegate
                    Button {
                        Text {
                            anchors.centerIn: parent
                            text: index
                            color: (GUIState.currentSlice === index)?"red":"black"
                        }
                        onClicked: GUIController.changeSlice(index)
                    }
                }
*/

                /* ================= Panel channelPanel ================= */
/*                TCCollapsiblePanel {
                    id: channelPanel
                    anchors { top: slicesPanel.bottom; left: parent.left; right: parent.right }
                    titleText : "channels"
                    state : "expanded"
                    model : GUIState.maximumChannel
                    delegate: channelDelegate
                }

                Component {
                    id: channelDelegate
                    Button {
                        Text {
                            anchors.centerIn: parent
                            text: index
                            color: (GUIState.currentChannel === index)?"red":"black"
                        }
                        onClicked: GUIController.changeChannel(index)
                    }
                }
*/
            }
        }
    }
}
