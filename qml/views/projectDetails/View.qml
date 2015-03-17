import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {

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

            Flickable {
                contentHeight: 600 - toolBar.height - statusBar.height
                anchors.fill: parent

                Loader {
                    id: loader
                    sourceComponent: generalView
                    anchors.fill: parent
                }
            }

            Component {
                id: generalView

                ColumnLayout {
                    id: general
                    anchors.margins: 10
                    anchors.fill: parent

                    property int fontSize: 14

                    Column {
                        spacing: 10
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        /*Text {
                            text: "project name"
                            font.pointSize: general.fontSize
                            height: 25
                            width: 180

                            TextField {
                                text: "Project_Test"
                                font.pointSize: general.fontSize
                                width: groupBox.width - 200
                                anchors.left: parent.right
                                anchors.leftMargin: 5
                            }
                        }*/

                        Text {
                            text: "project path"
                            font.pointSize: general.fontSize
                            height: 25
                            width: 180

                            TextField {
                                text: mousePosition.path
                                font.pointSize: general.fontSize
                                width: tableView.width - 280
                                readOnly: true
                                anchors.left: parent.right
                                anchors.leftMargin: 5

                                Button {
                                    text: "Browse ..."
                                    width: 80
                                    anchors.left: parent.right
                                    anchors.leftMargin: 5
                                }
                            }
                        }

                        /*Text {
                            text: "project status"
                            font.pointSize: general.fontSize
                            height: 25
                            width: 180

                            ComboBox {
                                width: 80
                                model: ["open", "closed"]
                                anchors.left: parent.right
                                anchors.leftMargin: 5
                            }
                        }*/

                        TableView {
                            id: tableView
                            //height: count > 0 ? contentHeight : 0
                            //delegate: editDelegate
                            //spacing: 5
                            anchors {
                                left: parent.left
                                right: parent.right
                            }

                            TableViewColumn {
                                role: "name"
                                title: "Name"
                                width: parent.width * 0.2
                            }

                            TableViewColumn {
                                role: "description"
                                title: "Description"
                                width: parent.width * 0.8
                            }

                            model: ListModel {

                                ListElement {
                                    name: "image directory"
                                    description: "/Users/enrico/Documents/Tracking/Daten/images"
                                }

                                ListElement {
                                    name: "metadata directory (XML)"
                                    description: "/Users/enrico/Documents/Tracking/Daten/xml"
                                }

                                ListElement {
                                    name: "track file (XML file)"
                                    description: "/Users/enrico/Documents/Tracking/Daten/tracksXML.xml"
                                }
                            }
                        }

                        /*GroupBox {
                            id: groupBox
                            anchors {
                                left: parent.left
                                right: parent.right
                            }

                            ListView {
                                height: count > 0 ? contentHeight : 0
                                delegate: editDelegate
                                spacing: 5

                                model: ListModel {

                                    ListElement {
                                        text: "image directory"
                                        value: "/Users/enrico/Documents/Tracking/Daten/images"
                                    }

                                    ListElement {
                                        text: "metadata directory (XML)"
                                        value: "/Users/enrico/Documents/Tracking/Daten/xml"
                                    }

                                    ListElement {
                                        text: "track file (XML file)"
                                        value: "/Users/enrico/Documents/Tracking/Daten/tracksXML.xml"
                                    }
                                }
                            }

                            Component {
                                id: editDelegate

                                Text {
                                    text: model.text
                                    font.pointSize: general.fontSize
                                    height: 25
                                    width: 180

                                    TextField {
                                        text: model.value
                                        font.pointSize: general.fontSize
                                        width: groupBox.width - 285
                                        readOnly: true
                                        anchors.left: parent.right
                                        anchors.leftMargin: 5

                                        Button {
                                            text: "Browse ..."
                                            width: 80
                                            anchors.left: parent.right
                                            anchors.leftMargin: 5
                                        }
                                    }
                                }
                            }
                        }*/

                        /*ListView {
                            height: count > 0 ? contentHeight : 0
                            delegate: textDelegate
                            anchors {
                                left: parent.left
                                right: parent.right
                            }

                            model: ListModel {

                                ListElement {
                                    text: "# image files:"
                                    value: "659"
                                }

                                ListElement {
                                    text: "# meta files:"
                                    value: "659"
                                }

                                ListElement {
                                    text: "# cells:"
                                    value: "6545"
                                }

                                ListElement {
                                    text: "# generated tracks:"
                                    value: "507"
                                }
                            }
                        }

                        Component {
                            id: textDelegate

                            Text {
                                text: model.text
                                font.pointSize: general.fontSize
                                width: 180

                                Text {
                                    text: model.value
                                    font.pointSize: general.fontSize
                                    anchors.left: parent.right
                                    anchors.leftMargin: 5
                                }
                            }
                        }*/
                    }

                    Button {
                        text: "Ok"
                        width: 80
                        anchors {
                            bottom: parent.bottom
                            right: parent.right
                        }

                        Button {
                            text: "Cancel"
                            width: 80
                            anchors.right: parent.left
                            anchors.rightMargin: 5
                        }
                    }
                }
            }

            Component {
                id: tracksView

                ColumnLayout {
                    id: newTracks
                    anchors.margins: 10
                    anchors.fill: parent

                    property int fontSize: 14

                    Column {
                        spacing: 10
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        GroupBox {
                            title: "general"
                            anchors {
                                left: parent.left
                                right: parent.right
                            }

                            Text {
                                text: "tracks:"
                                font.pointSize: newTracks.fontSize
                                width: 180

                                Text {
                                    text: "3"
                                    font.pointSize: newTracks.fontSize
                                    anchors.left: parent.right
                                    anchors.leftMargin: 5
                                }
                            }
                        }

                        GroupBox {
                            title: "average values"
                            anchors {
                                left: parent.left
                                right: parent.right
                            }

                            ListView {
                                height: count > 0 ? contentHeight : 0
                                delegate: textDelegate

                                model: ListModel {

                                    ListElement {
                                        text: "open-status tracks:"
                                        value: "2/3\t(66%)"
                                    }

                                    ListElement {
                                        text: "cell division-status tracks:"
                                        value: "0/3\t(0%)"
                                    }

                                    ListElement {
                                        text: "dead-status tracks:"
                                        value: "0/3\t(0%)"
                                    }

                                    ListElement {
                                        text: "lost-status tracks:"
                                        value: "1/3\t(33%)"
                                    }

                                    ListElement {
                                        text: "other-status tracks:"
                                        value: "0/3\t(0%)"
                                    }

                                    ListElement {
                                        text: "avg track length:"
                                        value: "138"
                                    }
                                }
                            }

                            Component {
                                id: textDelegate

                                Text {
                                    text: model.text
                                    font.pointSize: newTracks.fontSize
                                    width: 180

                                    Text {
                                        text: model.value
                                        font.pointSize: newTracks.fontSize
                                        anchors.left: parent.right
                                        anchors.leftMargin: 5
                                    }
                                }
                            }
                        }
                    }

                    Button {
                        text: "Ok"
                        width: 80
                        anchors {
                            bottom: parent.bottom
                            right: parent.right
                        }

                        Button {
                            text: "Cancel"
                            width: 80
                            anchors.right: parent.left
                            anchors.rightMargin: 5

                            Button {
                                text: "Refresh"
                                width: 80
                                anchors.right: parent.left
                                anchors.rightMargin: 15
                            }
                        }
                    }
                }
            }

            Component {
                id: timeView

                ColumnLayout {
                    id: requiredTime
                    anchors.margins: 10
                    anchors.fill: parent

                    property int fontSize: 14

                    Column {
                        spacing: 10
                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        Text {
                            text: "required time:"
                            font.pointSize: requiredTime.fontSize
                            width: 120

                            Text {
                                text: "260:18:936021\t(h:m:s)"
                                font.pointSize: requiredTime.fontSize
                                anchors.left: parent.right
                                anchors.leftMargin: 5
                            }
                        }

                        Text {
                            text: "elapsed days:"
                            font.pointSize: requiredTime.fontSize
                            width: 120

                            Text {
                                text: "56"
                                font.pointSize: requiredTime.fontSize
                                anchors.left: parent.right
                                anchors.leftMargin: 5
                            }
                        }

                        GroupBox {
                            title: "details"
                            anchors {
                                left: parent.left
                                right: parent.right
                            }

                            ListView {
                                height: count > 0 ? contentHeight : 0
                                delegate: textDelegate

                                model: ListModel {

                                    ListElement {
                                        text: "start"
                                        date: "Di, 11.11.2014"
                                        time: "21:26:32"
                                        duration: ""
                                    }

                                    ListElement {
                                        text: "end"
                                        date: "Di, 11.11.2014"
                                        time: "21:42:47"
                                        duration: "01:16:3615"
                                    }

                                    ListElement {
                                        text: "start"
                                        date: "Fr, 26.12.2014"
                                        time: "22:22:52"
                                        duration: ""
                                    }

                                    ListElement {
                                        text: "now"
                                        date: "Di, 06.01.2015"
                                        time: "17:24:58"
                                        duration: "259:02:932406"
                                    }
                                }
                            }

                            Component {
                                id: textDelegate

                                Text {
                                    text: model.text
                                    font.pointSize: requiredTime.fontSize
                                    width: 120

                                    Text {
                                        text: model.date
                                        font.pointSize: requiredTime.fontSize
                                        width: 120
                                        anchors.left: parent.right
                                        anchors.leftMargin: 5

                                        Text {
                                            text: model.time
                                            font.pointSize: requiredTime.fontSize
                                            width: 120
                                            anchors.left: parent.right
                                            anchors.leftMargin: 5

                                            Text {
                                                text: model.duration
                                                font.pointSize: requiredTime.fontSize
                                                anchors.left: parent.right
                                                anchors.leftMargin: 5
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    Button {
                        text: "Ok"
                        width: 80
                        anchors {
                            bottom: parent.bottom
                            right: parent.right
                        }

                        Button {
                            text: "Cancel"
                            width: 80
                            anchors.right: parent.left
                            anchors.rightMargin: 5
                        }
                    }
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

            GroupBox {
                anchors.margins: 20
                anchors.fill: parent

                Column {
                    spacing: 5

                    Button {
                        text: "General"
                        width: 245
                        onClicked: {
                            loader.sourceComponent = generalView
                        }
                    }

                    Button {
                        text: "New Tracks"
                        width: 245
                        onClicked: {
                            loader.sourceComponent = tracksView
                        }
                    }

                    Button {
                        text: "Cell Items"
                        width: 245
                    }

                    Button {
                        text: "Required Time"
                        width: 245
                        onClicked: {
                            loader.sourceComponent = timeView
                        }
                    }
                }
            }
        }
    }
}
