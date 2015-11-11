import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {
    /* This is the element for showing the tool bar of the tracking
       view. The tool buttons can be used to expand or collapse the
       sidebar, navigate through the frames and change the view. */
    width: window.width
    height: 50

    ToolBar {
        anchors.fill: parent

        RowLayout {
            anchors.fill: parent

            ListView {
                model: mainItem.stateList
                orientation: ListView.Horizontal
                spacing: 5
                anchors.fill: parent

                delegate: ToolButton {
                    /* Loads the selected view. */
                    id: viewButton
                    width: Math.max(nameText.width, buttonImage.width)
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom

                    ColumnLayout {
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom

                        Layout.alignment: Qt.AlignHCenter
                        Image {
                            id: buttonImage
                            source: model.iconPath
                            fillMode: Image.PreserveAspectFit
                            anchors.top: parent.top
                            anchors.bottom: nameText.top
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        Text {
                            id: nameText
                            anchors.bottom: parent.bottom
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: model.stateName
                            width: parent.width
                        }
                    }
                    onClicked: mainItem.state = model.stateName
                }
            }

            Item {
                Layout.fillWidth: true
            }

            ToolButton {
                /* Expands and collapses the sidebar. */
                height: parent.height
                width: height
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    right: parent.right
                }
                onClicked: {
                    if(sidebarIsExpanded)
                        sidebarIsExpanded = false;
                    else
                        sidebarIsExpanded = true;
                }

                Image {
                    source: sidebarIsExpanded ? "///icons/list-remove.png" : "///icons/list-add.png"
                    fillMode: Image.PreserveAspectFit
                    anchors.fill: parent
                    anchors.margins: parent.height * 0.1
                }
            }
        }
    }
}
