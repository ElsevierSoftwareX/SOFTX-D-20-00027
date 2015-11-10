import QtQuick 2.3
import QtQuick.Controls 1.3
import imb.celltracker 1.0

Menu {
    id: contextMenu
    visible: (objectAnnotationsModel.count + trackletAnnotationsModel.count) > 0

    ListModel {
        id: objectAnnotationsModel
        function rebuild() {
            // rebuild the model
            var annotations = DataProvider.annotations;

            objectAnnotationsModel.clear();
            for (var i=0; i<annotations.length; i++)
                if (annotations[i].type === Annotation.OBJECT_ANNOTATION)
                    objectAnnotationsModel.append({"title" : annotations[i].title, "id" : annotations[i].id})
            objectAnnotationsModel.countChanged()
        }
        Component.onCompleted: rebuild()
    }

    ListModel {
        id: trackletAnnotationsModel
        function rebuild() {
            // rebuild the model
            var annotations = DataProvider.annotations;

            trackletAnnotationsModel.clear();
            for (var i=0; i<annotations.length; i++)
                if (annotations[i].type === Annotation.TRACKLET_ANNOTATION)
                    trackletAnnotationsModel.append({"title" : annotations[i].title, "id" : annotations[i].id})
            trackletAnnotationsModel.countChanged()
        }
        Component.onCompleted: rebuild()
    }

    Connections {
        target: DataProvider
        onAnnotationsChanged: {
            console.log("annotationsChanged")
            objectAnnotationsModel.rebuild()
        }
    }

    MenuItem {
        text: "Object Annotations"
        visible: objectAnnotationsModel.count > 0
        enabled: false
    }

    Instantiator {
        model: objectAnnotationsModel
        onObjectAdded: contextMenu.insertItem(index,object)
        onObjectRemoved: contextMenu.removeItem(object)

        MenuItem {
            text: model.id + ": " + model.title
            checkable: true
        }
    }

    MenuSeparator {
        visible: (objectAnnotationsModel.count > 0) && (trackletAnnotationsModel.count > 0)
    }

    MenuItem {
        text: "Tracklet Annotations"
        visible: trackletAnnotationsModel.count > 0
        enabled: false
    }

    Instantiator {
        model: trackletAnnotationsModel
        onObjectAdded: contextMenu.insertItem(index,object)
        onObjectRemoved: contextMenu.removeItem(object)

        MenuItem {
            text: model.id + ": " + model.title
            checkable: true
        }
    }
}
