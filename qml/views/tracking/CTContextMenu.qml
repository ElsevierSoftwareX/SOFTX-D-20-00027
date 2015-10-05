import QtQuick 2.3
import QtQuick.Controls 1.3
import imb.celltracker 1.0

Menu {
    id: contextMenu

    ListModel {
        id: annotationsModel
    }

    onPopupVisibleChanged: {
        // rebuild the model
        var annotations = DataProvider.annotations;

        annotationsModel.clear();
        for (var i=0; i<annotations.length; i++)
            annotationsModel.append({"name" : annotations[i].title})
    }

    Instantiator {
        model: annotationsModel
        onObjectAdded: contextMenu.insertItem(index,object)
        onObjectRemoved: contextMenu.removeItem(object)

        MenuItem {
            text: model.name
            /* the triggers somehow don't work under linux, but do so under OSX.
                                     * was tested with Qt 5.4.2 and 5.5.0 using clang and gcc
                                     * \todo: Investigate why and fix the problem
                                     */
            onTriggered: console.log("MenuItem" + model.name + " triggered")
        }
    }

    MenuSeparator {
        visible: annotationsModel.count > 2
    }

    MenuItem {
        id: addObjectAnnotation
        text: "Add object annotation"
        onTriggered: {
            console.log("Now let's add a new object annotation");
        }
    }

    MenuItem {
        id: addTrackAnnotation
        text: "Add track annotation"
        onTriggered: {
            console.log("Now let's add a new track annotation")
        }
    }
}
