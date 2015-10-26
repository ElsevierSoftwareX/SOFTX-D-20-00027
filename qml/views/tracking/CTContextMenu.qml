import QtQuick 2.3
import QtQuick.Controls 1.3
import imb.celltracker 1.0

Menu {
    id: contextMenu

    ListModel {
        id: annotationsModel
        function rebuild() {
            // rebuild the model
            var annotations = DataProvider.annotations;

            annotationsModel.clear();
            for (var i=0; i<annotations.length; i++)
                annotationsModel.append({"title" : annotations[i].title, "id" : annotations[i].id})
            annotationsModel.countChanged()
        }
        Component.onCompleted: rebuild()
    }

    Connections {
        target: DataProvider
        onAnnotationsChanged: {
            console.log("annotationsChanged")
            annotationsModel.rebuild()
        }
    }

    Instantiator {
        model: annotationsModel
        onObjectAdded: contextMenu.insertItem(index,object)
        onObjectRemoved: contextMenu.removeItem(object)

        MenuItem {
            text: id + ": " + title
            checkable: true
            onCheckedChanged: console.log("MenuItem " + model.title + " " + (checked?"checked":"unchecked"))
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
