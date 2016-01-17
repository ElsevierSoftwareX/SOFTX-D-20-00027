import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import imb.celltracker 1.0

MenuBar {
    /* This is the element for showing a menu bar, but only those
       entries are visible that belong to the current view. */
    property bool trackingViewIsVisible: mainItem.state === "Tracking"
    property bool testViewIsVisible: mainItem.state === "TestView"

    Menu {
        /* The file menu calls the dialogs for creating
           new projects or open a chosen HDF5 file. */
        title: "File"
        enabled: trackingViewIsVisible
        visible: trackingViewIsVisible

        MenuItem {
            text: "New Project"
            shortcut: StandardKey.New
            onTriggered: dialog.visible = true
        }

        MenuItem {
            text: "Open Project"
            shortcut: StandardKey.Open
            onTriggered: loadFileDialog.visible = true
        }

        MenuSeparator {}

        MenuItem {
            text: "Save Project"
            shortcut: StandardKey.Save
            onTriggered: {
                GUIState.mouseAreaActive = false
                DataProvider.saveHDF5()
            }
        }

        MenuItem {
            text: "Save Project As"
            shortcut: StandardKey.SaveAs
            onTriggered: saveFileDialog.visible = true
        }

        MenuSeparator {}

        /*Menu {
            title: "Export"

            MenuItem {
                text: "Export New Tracks To XML"
            }
        }*/

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
            text: "Object Info"
            checkable: true
        }

        MenuItem {
            text: "Track Info"
            checkable: true
        }

        MenuItem {
            text: "Strategies"
            checkable: true
        }

        MenuItem {
            text: "Operations"
            checkable: true
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
        title: "File"
        enabled: testViewIsVisible
        visible: testViewIsVisible

        MenuItem {
            text: "bla"
        }

        MenuItem {
            text: "Exit"
            onTriggered: Qt.quit()
        }
    }

    Menu {
        title: "View"
        enabled: testViewIsVisible
        visible: testViewIsVisible

        MenuItem {
            text: "bla"
        }
    }

    Menu {
        title: "Window"
        enabled: testViewIsVisible
        visible: testViewIsVisible

        MenuItem {
            text: "bla"
        }
    }

    Menu {
        title: "Help"

        MenuItem {
            text: "Show Desktop Info"
        }

        MenuItem {
            text: "How To Do (Tutorial)"
        }
    }
}
