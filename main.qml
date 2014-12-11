import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

ApplicationWindow {
    id: window
    title: "CellTracker"
    visible: true
    width: 800
    height: 600
    menuBar: currentView == 1 ? menuBar1 : menuBar2

    property int currentView: 1
    property bool sidebarIsExpanded: true

    MenuBar1 {
        id: menuBar1
    }

    MenuBar2 {
        id: menuBar2
    }

    toolBar: ListView {
        anchors.fill: parent
        model: currentView == 1 ? model1 : model2
        delegate: currentView == 1 ? toolBar1 : toolBar2

        ToolBar1 {
            id: toolBar1
        }

        ToolBar2 {
            id: toolBar2
        }
    }

    ListView {
        height: window.height * 0.9
        model: currentView == 1 ? model1 : model2
        delegate: currentView == 1 ? view1 : view2
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        View1 {
            id: view1
        }

        View2 {
            id: view2
        }
    }

    statusBar: StatusBar {

        ListView {
            model: currentView == 1 ? model1 : model2
            delegate: currentView == 1 ? statusBar1 : statusBar2
            anchors {
                top: parent.top
                bottom: parent.bottom
            }

            Component {
                id: statusBar1

                RowLayout {
                    Label {
                        text: model.statusbarText
                    }
                }
            }

            Component {
                id: statusBar2

                RowLayout {
                    Label {
                        text: model.statusbarText
                    }
                }
            }
        }
    }

    Model1 {
        id: model1
    }

    Model2 {
        id: model2
    }
}
