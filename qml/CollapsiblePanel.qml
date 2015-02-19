import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Rectangle {
    id: collapsiblePanel
    height: titleRect.height + listView.height
    state: "collapsed"
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
    property alias header: listView.header
    property alias model: listView.model
    property alias delegate: listView.delegate

    Rectangle {
        id: titleRect
        width: parent.width
        height: 40

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if(collapsiblePanel.state == "collapsed") collapsiblePanel.state = "expanded"
                else collapsiblePanel.state = "collapsed"
            }
        }

        Image {
            id: titleArrow
            width: 20
            height: 20
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
            color: "#c1c3c8"
        }
    }

    ListView {
        id: listView
        height: count > 0 ? contentHeight + 20 : 0
        spacing: 5
        anchors.margins: 10
        anchors {
            top: titleRect.bottom
            left: parent.left
            right: parent.right
        }
    }
}
