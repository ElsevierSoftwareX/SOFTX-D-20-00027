import QtQuick 2.3
import QtQuick.Controls 1.3
import imb.celltracker 1.0

Menu {
    id: contextMenu

    ListModel {
        id: objectAnnotationsModel
        function rebuild() {
            // rebuild the model
            var annotations = DataProvider.annotations;

            objectAnnotationsModel.clear();
            for (var i=0; i<annotations.length; i++) {
                if (annotations[i].type === Annotation.OBJECT_ANNOTATION)
                    objectAnnotationsModel.append({   "title" : annotations[i].title,
                                                      "id" : annotations[i].id,
                                                      "annotated" : DataProvider.isAnnotatedWith(annotations[i].id)});
            }
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
            for (var i=0; i<annotations.length; i++) {
                if (annotations[i].type === Annotation.TRACKLET_ANNOTATION) {
                    trackletAnnotationsModel.append({ "title" : annotations[i].title,
                                                      "id" : annotations[i].id,
                                                      "annotated" : DataProvider.isAnnotatedWith(annotations[i].id)});
                }
            }
            trackletAnnotationsModel.countChanged()
        }
        Component.onCompleted: rebuild()
    }

    function rebuildModels() {
        objectAnnotationsModel.rebuild()
        trackletAnnotationsModel.rebuild()
    }

    Connections {
        target: DataProvider
        onAnnotationsChanged: rebuildModels()
    }
    Connections {
        target: GUIState
        onSelectedTrackChanged: rebuildModels()
        onSelectedCellChanged: rebuildModels()
    }

    MenuItem {
        text: "No Annotations"
        enabled: false
        visible: (objectAnnotationsModel.count + trackletAnnotationsModel.count) === 0
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
            checked: model.annotated
            checkable: true
            onTriggered: DataProvider.toggleAnnotate(model.id)
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
            checked: model.annotated
            checkable: true
            onTriggered: DataProvider.toggleAnnotate(model.id)
        }
    }
}
