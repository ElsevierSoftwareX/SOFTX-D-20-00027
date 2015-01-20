import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {
    id: mainItem
    state: "TrackingView"
    states: [

        State {
            name: "TrackingView"

            PropertyChanges {
                target: mainItem
                //menuBar: "views/tracking/MenuBar.qml"
                view: "views/tracking/View.qml"
                toolBar: "views/tracking/ToolBar.qml"
                statusBar: "views/tracking/StatusBar.qml"
            }
        },

        State {
            name: "ProjectDetails"

            PropertyChanges {
                target: mainItem
                view: "views/projectDetails/View.qml"
                toolBar: "views/projectDetails/ToolBar.qml"
                statusBar: "views/projectDetails/StatusBar.qml"
            }
        },

        State {
            name: "TestView"

            PropertyChanges {
                target: mainItem
                //menuBar: "views/test/MenuBar.qml"
                view: "views/test/View.qml"
                toolBar: "views/test/ToolBar.qml"
                statusBar: "views/test/StatusBar.qml"
            }
        }
    ]

    property bool sidebarIsExpanded: true
    //property string menuBar: "views/tracking/MenuBar.qml"
    property string toolBar: "views/tracking/ToolBar.qml"
    property string view: "views/tracking/View.qml"
    property string statusBar: "views/tracking/StatusBar.qml"

    ApplicationWindow {
        id: window
        title: "CellTracker"
        visible: true
        width: 900
        height: 600

        Loader {
            source: "MenuBar.qml"
        }

        toolBar: Loader {
            anchors.fill: parent
            source: mainItem.toolBar
        }

        Loader {
            height: window.height * 0.9
            source: mainItem.view
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
        }

        statusBar: StatusBar {

            Loader {
                source: mainItem.statusBar
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }
            }
        }
    }
}
