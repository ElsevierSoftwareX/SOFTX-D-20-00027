import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1
import imb.celltracker 1.0

MenuBar {
    /* This is the element for showing a menu bar, but only those
       entries are visible that belong to the current view. */
    property bool trackingViewIsVisible: mainItem.state === "Tracking"

    Menu {
        /* The file menu calls the dialogs for creating
           new projects or open a chosen HDF5 file. */
        title: "File"

        MenuItem {
            text: "Open Project"
            shortcut: StandardKey.Open
            onTriggered: loadFileDialog.visible = true
        }

        MenuSeparator {}

        MenuItem {
            text: "Save Project"
            enabled: GUIState.projPath !== ""
            shortcut: StandardKey.Save
            onTriggered: {
                statusWindow.visible = true
                GUIState.mouseAreaActive = false
                DataProvider.saveHDF5()
            }
        }

        MenuItem {
            text: "Save Project As"
            enabled: GUIState.projPath !== ""
            shortcut: StandardKey.SaveAs
            onTriggered: saveFileDialog.visible = true
        }

        MenuSeparator {}

        MenuItem {
            text: "Export..."
            enabled: GUIState.projPath !== ""
            onTriggered: exportDialog.visible = true
        }

        MenuSeparator {}

        MenuItem {
            text: "Exit"
            onTriggered: Qt.quit()
        }
    }

    Menu {
        title: "View"
        enabled: trackingViewIsVisible
        visible: trackingViewIsVisible

        MenuItem {
            text: "Object Outlines"
            shortcut: "Ctrl+D"
            checkable: true
            checked: GUIState.drawOutlines
            onTriggered: {
                GUIState.drawOutlines = !GUIState.drawOutlines;
                GUIState.backingDataChanged();
            }
        }

        MenuItem {
            text: "Tracklet IDs"
            shortcut: "Ctrl+T"
            checkable: true
            checked: GUIState.drawTrackletIDs
            onTriggered: {
                GUIState.drawTrackletIDs = !GUIState.drawTrackletIDs;
                GUIState.backingDataChanged();
            }
        }

        MenuItem {
            text: "Annotation Info"
            shortcut: "Ctrl+A"
            checkable: true
            checked: GUIState.drawAnnotationInfo
            onTriggered: {
                GUIState.drawAnnotationInfo = !GUIState.drawAnnotationInfo;
                GUIState.backingDataChanged();
            }
        }

        MenuSeparator {}

        MenuItem {
            text: "Zoom In"
            shortcut: StandardKey.ZoomIn
            onTriggered: {
                var zoomDiff = 1.05
                GUIState.zoomFactor *= zoomDiff
                GUIState.offX += (1-zoomDiff)* GUIState.mouseX * GUIState.zoomFactor
                GUIState.offY += (1-zoomDiff)* GUIState.mouseY * GUIState.zoomFactor
            }

        }

        MenuItem {
            text: "Zoom Out"
            shortcut: StandardKey.ZoomOut
            onTriggered: {
                var zoomDiff = 1/1.05
                GUIState.zoomFactor *= zoomDiff
                GUIState.offX += (1-zoomDiff)* GUIState.mouseX * GUIState.zoomFactor
                GUIState.offY += (1-zoomDiff)* GUIState.mouseY * GUIState.zoomFactor
            }
        }

        MenuItem {
            text: "Zoom Normal"
            shortcut: "Ctrl+0"
            onTriggered: {
                GUIState.zoomFactor = 1.0
                GUIState.offX = 0
                GUIState.offY = 0
            }
        }
    }

    Menu {
        title: "Help"

        MenuItem {
            text: "Not yet"
            enabled: false
        }
    }
}
