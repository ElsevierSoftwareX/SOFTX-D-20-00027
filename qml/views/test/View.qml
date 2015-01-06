import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

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

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    Qt.quit();
                }
            }

            Text {
                text: qsTr("Workspace area 2")
                anchors.centerIn: parent
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

            Text {
                text: qsTr("Sidebar 2")
                anchors.centerIn: parent
            }
        }
    }
}
