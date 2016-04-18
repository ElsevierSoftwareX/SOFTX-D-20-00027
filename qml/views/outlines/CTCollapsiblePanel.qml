import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Window 2.1

Rectangle {
    /* This is a template element for a collapsible panel that is
       used in the sidebar of the tracking view. Model, delegate,
       and title text can be added. */
    id: collapsiblePanel
    height: titleRect.height + listView.height
    state: "collapsed"
    color: "transparent"
    states: [

        State {
            name: "collapsed"

            PropertyChanges {
                target: listView
                height: 0
                opacity: 0
            }

            PropertyChanges {
                target: titleArrow
                rotation: -90
            }

            PropertyChanges {
                target: collapsiblePanel
                height: titleRect.height
            }
        }
    ]

    property alias titleText: titleTxt.text
    property alias model: listView.model
    property alias delegate: listView.delegate
    property alias header: listView.header

    Rectangle {
        id: titleRect
        width: parent.width
        height: 40
        color: "transparent"

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if(collapsiblePanel.state == "collapsed") collapsiblePanel.state = "expanded"
                else collapsiblePanel.state = "collapsed"
            }
        }

        Image {
            id: titleArrow
            width: 10
            height: 10
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.verticalCenter: parent.verticalCenter
            source: "///qml/views/tracking/icons/arrow-down.png"
        }

        Text {
            id: titleTxt
            anchors.left: titleArrow.right
            anchors.leftMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            font.bold: true
            font.pixelSize: 18
            color: Qt.rgba(0,0,0,0.25)
        }
    }

    ListView {
        id: listView
        height: count > 0 ? contentHeight : 0
        width: parent.width

        anchors.margins: 0
        anchors {
            top: titleRect.bottom
            left: parent.left
            right: parent.right
        }
    }
}