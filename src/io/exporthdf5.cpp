#include "exporthdf5.h"

#include <set>
#include <H5Cpp.h>
#include <QDebug>
#include <QFile>

#include "tracked/trackevent.h"
#include "tracked/trackeventdead.h"
#include "tracked/trackeventdivision.h"
#include "tracked/trackeventlost.h"
#include "tracked/trackeventmerge.h"
#include "tracked/trackeventunmerge.h"
#include "hdf5_aux.h"
#include "exceptions/ctexportexception.h"
#include "provider/messagerelay.h"

/*!
{                                                                       -> save
    "annotations": {                                                            -> saveAnnotations
        "object_annotations": {                                                 "
            "0" : {                                                                     -> saveAnnotation
                "description": "Description for object annotation",                     "
                "object_annotation_id": 0,                                              "
                "title": "Title of object annotation"                                   "
            },                                                                          "
            "…" : {},                                                                   -> saveAnnotation
            "n" : {}                                                                    -> saveAnnotation
        },                                                                      "
        "track_annotations": {                                                  "
            "0" : {                                                                     -> saveAnnotation
                "description": "Description for object annotation",                     "
                "track_annotation_id": 0,                                               "
                "title": "Title of object annotation"                                   "
            },                                                                          "
            "…" : {},                                                                   -> saveAnnotation
            "n" : {}                                                                    -> saveAnnotation
        }                                                                       "
    },                                                                          "
*/
/*!
    "events": {
        "celldead": {
            "description": "Description of event",
            "event_id": 0,
            "name": "Name of event"
        },
        "celldivision": {
            "description": "Description of event",
            "event_id": 1,
            "name": "Name of event"
        },
        "celllost": {
            "description": "Description of event",
            "event_id": 2,
            "name": "Name of event"
        },
        "cellmerge": {
            "description": "Description of event",
            "event_id": 3,
            "name": "Name of event"
        },
        "cellunmerge": {
            "description": "Description of event",
            "event_id": 4,
            "name": "Name of event"
        },
        "endofmovie": {
            "description": "Description of event",
            "event_id": 5,
            "name": "Name of event"
        }
    },
*/
/*!
    "objects": {
        "frames": {
            "0": {
                "slices": {
                    "0": {
                        "channels": {
                            "0": {
                                "objects": {
                                    "0": {
                                        "annotations": {
                                            "0": "->/annotations/object_annotations/0",
                                            "…": "->/annotations/object_annotations/…",
                                            "n": "->/annotations/object_annotations/n"
                                        },
                                        "bounding_box": [[45,34],[100,50]],
                                        "centroid": [75,42],
                                        "channel_id": 0,
                                        "frame_id": 0,
                                        "object_id": 0,
                                        "outline": [[75,42],[75,42],[75,42],[75,42],[75,42],[75,42]],
                                        "packed_mask": "10b20w40b10w",
                                        "slice_id": 0
                                    },
                                    "…": {},
                                    "n": {}
                                },
                                "channel_id": 0
                            },
                            "…": {},
                            "n": {}
                        },
                        "dimensions": "Image dimensions",
                        "nchannels": 3,
                        "slice_id": 0
                    },
                    "…": {},
                    "n": {}
                },
                "frame_id": 0
            },
            "…": {},
            "n": {}
        },
        "frame_rate": 60,
        "nframes": 1500,
        "nslices": 12,
        "slice_shape": [3,4]
    },
*/
/*!
    "tracklets": {                                                      -> saveTracklets
        "0": {                                                          "
            "contained": {                                                      -> saveTrackletsContained
                "0": "->/objects/frames/0/slices/0/channels/0/4",               "
                "1": "->/objects/frames/1/slices/0/channels/0/6",               "
                "…": "->/objects/frames/1/slices/0/channels/0/…",               "
                "n": "->/objects/frames/1/slices/0/channels/0/n"                "
            },                                                          "
            "annotations": {                                                    -> saveTrackletsAnnotations
                "0": "->/annotations/track_annotations/1",                      "
                "…": "->/annotations/track_annotations/…",                      "
                "n": "->/annotations/track_annotations/m"                       "
            },                                                                  "
            "tracklet_id": 0,                                           "
            "next_event": "->/events/cell_division/",                           -> saveTrackletsNextEvent
            "next": {                                                           "
                "0": "->/tracklets/100",                                        "
                "1": "->/tracklets/200"                                         "
            },                                                                  "
            "previous_event": "->/events/cell_division/",                       -> saveTrackletsPreviousEvent
            "previous": {                                                       "
                "0": "->/autotracklets/100"                                     "
            },                                                                  "
            "start": 0,                                                 "
            "end": 10                                                   "
        },                                                              "
        "…": {},                                                        -> saveTracklets
        "n": {}                                                         -> saveTracklets
    }
}

*/

namespace CellTracker {
using namespace H5;

ExportHDF5::ExportHDF5() { }
ExportHDF5::~ExportHDF5() { }

bool ExportHDF5::save(std::shared_ptr<Project> project, QString filename)
{
    if (project->getFileName().compare(filename) != 0) {
        /* save to a different file than where this project was loaded from */
        QFile oldFile(project->getFileName());
        oldFile.copy(filename);
        oldFile.close();
    }

    try {
        H5File file(filename.toStdString().c_str(), H5F_ACC_RDWR, H5P_FILE_CREATE);

        MessageRelay::emitUpdateOverallName("Exporting to HDF5");
        MessageRelay::emitUpdateOverallMax(3);

        qDebug() << "Saving annotations";
        if (!saveAnnotations(file, project))
            throw CTExportException("Saving the annotations failed");
        MessageRelay::emitIncreaseOverall();

        qDebug() << "Saving tracklets";
        if (!saveTracklets(file, project))
            throw CTExportException("Saving the tracklets failed");
        MessageRelay::emitIncreaseOverall();

        qDebug() << "Saving events";
        if (!saveEvents(file, project))
            throw CTExportException("Saving the events failed");
        MessageRelay::emitIncreaseOverall();
    } catch (FileIException &e) {
        throw CTExportException("Saving the HDF5 file failed: " + e.getDetailMsg());
    }

    return true;
}

bool ExportHDF5::saveEvent(H5File file, std::shared_ptr<TrackEvent<Tracklet>> t, int evId)
{
    std::string path;
    QList<std::shared_ptr<Tracklet>> next;
    QList<std::shared_ptr<Tracklet>> prev;
    /* do the big switch case statement */
    switch (t->getType()) {
    case TrackEvent<Tracklet>::EVENT_TYPE_DEAD: {
        std::shared_ptr<TrackEventDead<Tracklet>> ev = std::static_pointer_cast<TrackEventDead<Tracklet>>(t);
        path = "/events/cell death/";
        prev.push_back(ev->getPrev()); }
        break;
    case TrackEvent<Tracklet>::EVENT_TYPE_DIVISION: {
        std::shared_ptr<TrackEventDivision<Tracklet>> ev = std::static_pointer_cast<TrackEventDivision<Tracklet>>(t);
        path = "/events/cell division/";
        prev.push_back(ev->getPrev());
        for (std::shared_ptr<Tracklet> tr : *ev->getNext())
            next.push_back(tr); }
        break;
    case TrackEvent<Tracklet>::EVENT_TYPE_LOST: {
        std::shared_ptr<TrackEventLost<Tracklet>> ev = std::static_pointer_cast<TrackEventLost<Tracklet>>(t);
        path = "/events/cell lost/";
        prev.push_back(ev->getPrev()); }
        break;
    case TrackEvent<Tracklet>::EVENT_TYPE_MERGE: {
        std::shared_ptr<TrackEventMerge<Tracklet>> ev = std::static_pointer_cast<TrackEventMerge<Tracklet>>(t);
        path = "/events/cell merge/";
        for (std::shared_ptr<Tracklet> tr: *ev->getPrev())
            prev.push_back(tr);
        next.push_back(ev->getNext()); }
        break;
    case TrackEvent<Tracklet>::EVENT_TYPE_UNMERGE: {
        std::shared_ptr<TrackEventUnmerge<Tracklet>> ev = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(t);
        path = "/events/cell unmerge/";
        prev.push_back(ev->getPrev());
        for (std::shared_ptr<Tracklet> tr : *ev->getNext())
            next.push_back(tr); }
        break;
    }

    Group currentGroup = file.openGroup(path);
    std::string evName = std::to_string(evId);
    Group eventGroup = openOrCreateGroup(currentGroup, evName.c_str());

    if (!prev.isEmpty()) {
        Group prevGrp = openOrCreateGroup(eventGroup, "prev");
        for (std::shared_ptr<Tracklet> t : prev) {
            std::string id = std::to_string(t->getID());
            std::string target = "/tracklets/" + id;
            linkOrOverwriteLink(H5L_TYPE_SOFT, prevGrp, target, id);
        }
    }
    if (!next.isEmpty()) {
        Group prevGrp = openOrCreateGroup(eventGroup, "next");
        for (std::shared_ptr<Tracklet> t : next) {
            std::string id = std::to_string(t->getID());
            std::string target = "/tracklets/" + id;
            linkOrOverwriteLink(H5L_TYPE_SOFT, prevGrp, target, id);
        }
    }
    return true;
}

bool ExportHDF5::saveEvents(H5File file, std::shared_ptr<Project> project)
{
    std::set<std::shared_ptr<TrackEvent<Tracklet>>> events;
    /* find all events */
    if (!project)
        return false;
    std::shared_ptr<Genealogy> gen = project->getGenealogy();
    if (!gen)
        return false;
    std::shared_ptr<QHash<int, std::shared_ptr<Tracklet>>> ts = gen->getTracklets();
    if (!ts)
        return false;

    QList<std::shared_ptr<Tracklet>> tracklets = ts->values();
    for (std::shared_ptr<Tracklet> t : tracklets) {
        std::shared_ptr<TrackEvent<Tracklet>> ev;
        ev = t->getPrev();
        if (ev)
            events.insert(ev);
        ev = t->getNext();
        if (ev)
            events.insert(ev);
    }

    MessageRelay::emitUpdateDetailName("Saving events");
    MessageRelay::emitUpdateDetailMax(events.size());

    clearOrCreateGroup(file, "/events/cell dead/");
    clearOrCreateGroup(file, "/events/cell division/");
    clearOrCreateGroup(file, "/events/cell lost/");
    clearOrCreateGroup(file, "/events/cell merge/");
    clearOrCreateGroup(file, "/events/cell unmerge/");

    bool allGood = true;
    int id = 0;
    for (std::shared_ptr<TrackEvent<Tracklet>> t : events) {
        allGood &= saveEvent(file, t, id++);
        MessageRelay::emitIncreaseDetail();
    }

    return allGood;
}

bool ExportHDF5::saveTrackletsContained(H5File file, Group grp, std::shared_ptr<Tracklet> t) {
    Group containedGroup = grp.createGroup("contained");
    QHash<int,QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> contained = t->getContained();

    /* sort the tracklets */
    QList<QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> ps = contained.values();
    qSort(ps.begin(), ps.end(),
          [](const QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> a, const QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> b) -> bool {
        return (a.first->getID() != b.first->getID())?
                    (a.first->getID() > b.first->getID()):
                    (a.second->getId() > b.second->getId());
    });

    /* write objects of this tracklet */
    for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> pair : ps) {
        uint32_t fId = pair.first->getID();
        uint32_t oId = pair.second->getId();
        uint32_t sId = pair.second->getSliceId();
        uint32_t cId = pair.second->getChannelId();

        std::string target = "/objects/frames/"+std::to_string(fId)
                +"/slices/"+std::to_string(sId)
                +"/channels/"+std::to_string(cId)
                +"/"+std::to_string(oId);
        if(!linkExists(file, target.c_str()))
            qDebug() << target.c_str() << "does not exist";
        linkOrOverwriteLink(H5L_TYPE_SOFT, containedGroup, target, std::to_string(fId));
    }
    return true;
}

bool ExportHDF5::saveTrackletsAnnotations(H5File file, Group grp, std::shared_ptr<Tracklet> t) {
    std::shared_ptr<QList<std::shared_ptr<Annotation>>> annotations = t->getAnnotations();

    Group annotationsGroup = grp.createGroup("annotations", annotations->length());

    for (std::shared_ptr<Annotation> a : *annotations) {
        std::string target = "/annotations/track_annotations/" + std::to_string(a->getId());
        if(!linkExists(file, target.c_str()))
            qDebug() << target.c_str() << "does not exist";
        linkOrOverwriteLink(H5L_TYPE_SOFT, annotationsGroup, target, std::to_string(a->getId()));
    }
    return true;
}
bool ExportHDF5::saveTrackletsNextEvent(Group grp, std::shared_ptr<Tracklet> t) { }
bool ExportHDF5::saveTrackletsPreviousEvent(Group grp, std::shared_ptr<Tracklet> t) { }

bool ExportHDF5::saveTracklets(H5File file, std::shared_ptr<Project> project)
{
    std::shared_ptr<QHash<int,std::shared_ptr<Tracklet>>> tracklets = project->getGenealogy()->getTracklets();
    Group trackletsGroup = openOrCreateGroup(file, "/tracklets", tracklets->size());

    MessageRelay::emitUpdateDetailName("Saving tracklets");
    MessageRelay::emitUpdateDetailMax(tracklets->size());

    for (std::shared_ptr<Tracklet> t: *tracklets) {
        bool hasAnnotations = (t->getAnnotations()->length() > 0);
        bool hasNextEvent = (t->getNext() != nullptr);
        bool hasPreviousEvent = (t->getPrev() != nullptr);

        int size = 4                            /* start, end, tracklet_id, contained */
                 + ((hasAnnotations)?1:0)       /* annotations-Group */
                 + ((hasNextEvent)?2:0)         /* next_event + next-Group */
                 + ((hasPreviousEvent)?2:0);    /* previous_event + previous-Group */

        /* we don't want objects of old tracklets lying around */
        Group trackletGroup = clearOrCreateGroup(trackletsGroup, std::to_string(t->getID()).c_str(), size);

        /* write id of this tracklet, start and end */
        writeSingleValue<uint32_t>(t->getID(), trackletGroup, "tracklet_id", PredType::NATIVE_UINT32);
        writeSingleValue<uint32_t>(t->getStart().first->getID(), trackletGroup, "start", PredType::NATIVE_UINT32);
        writeSingleValue<uint32_t>(t->getEnd().first->getID(), trackletGroup, "end", PredType::NATIVE_UINT32);

        /* write the links to the objects contained by this tracklet */
        saveTrackletsContained(file, trackletGroup, t);

        /* write the links to the annotations of this tracklet, if it has annotations */
        if (hasAnnotations)
            saveTrackletsAnnotations(file, trackletGroup, t);

        /* write the links to the next_event, create next-Group and fill it with links to tracklets, if it has a next event */
        if (hasNextEvent)
            saveTrackletsNextEvent(trackletGroup, t);

        /* write the links to the previous_event, create previous-Group and fill it with links to tracklets, if it has a previous event */
        if (hasPreviousEvent)
            saveTrackletsPreviousEvent(trackletGroup, t);


        MessageRelay::emitIncreaseDetail();
    }
    return true;
}

bool ExportHDF5::saveAnnotation(Group grp, std::shared_ptr<Annotation> a)
{
    StrType st(PredType::C_S1, H5T_VARIABLE);
    Group aGroup = grp.createGroup(std::to_string(a->getId()), 3);
    writeSingleValue<uint32_t>(a->getId(), aGroup, "id", PredType::NATIVE_UINT32);
    writeSingleValue<std::string>(a->getTitle().toStdString().c_str(), aGroup, "title", st);
    writeSingleValue<std::string>(a->getDescription().toStdString().c_str(), aGroup, "description", st);
    return true;
}

bool ExportHDF5::saveAnnotations(H5File file, std::shared_ptr<Project> project)
{
    std::shared_ptr<QList<std::shared_ptr<Annotateable>>> allAnnotated = project->getGenealogy()->getAnnotated();
    std::shared_ptr<QList<std::shared_ptr<Annotation>>> allAnnotations = project->getGenealogy()->getAnnotations();
    Group annotationsGroup = clearOrCreateGroup(file, "/annotations");

    MessageRelay::emitUpdateDetailName("Saving annotations");
    MessageRelay::emitUpdateDetailMax(allAnnotations->size());

    QMultiMap<std::shared_ptr<Annotation>,std::shared_ptr<Annotateable>> objectAnnotations;
    QMultiMap<std::shared_ptr<Annotation>,std::shared_ptr<Annotateable>> trackAnnotations;
    QMultiMap<std::shared_ptr<Annotation>,std::shared_ptr<Annotateable>> otherAnnotations;

    for (std::shared_ptr<Annotateable> annotated : *allAnnotated) {
        switch (annotated->getAnnotationType()) {
        case Annotateable::OBJECT_ANNOTATION:
            for (std::shared_ptr<Annotation> annotation : *annotated->getAnnotations())
                objectAnnotations.insert(annotation, annotated);
            break;
        case Annotateable::TRACKLET_ANNOTATION:
            for (std::shared_ptr<Annotation> annotation : *annotated->getAnnotations())
                trackAnnotations.insert(annotation, annotated);
            break;
        }
    }

    /* find all remaining annotations, that are neither associated with a obejct nor a tracklet */
    for (std::shared_ptr<Annotation> annotation : *allAnnotations) {
        if (!objectAnnotations.keys().contains(annotation) && !trackAnnotations.keys().contains(annotation))
            otherAnnotations.insert(annotation,nullptr);
    }

    { /* object annotations */
        Group oAnno = clearOrCreateGroup(annotationsGroup, "object_annotations", objectAnnotations.size());
        for (std::shared_ptr<Annotation> a : objectAnnotations.keys()) {
            saveAnnotation(oAnno, a);
            MessageRelay::emitIncreaseDetail();
        }
    }

    { /* track annotations */
        Group tAnno = clearOrCreateGroup(annotationsGroup, "track_annotations", trackAnnotations.size());
        for (std::shared_ptr<Annotation> a : trackAnnotations.keys()) {
            saveAnnotation(tAnno, a);
            MessageRelay::emitIncreaseDetail();
        }
    }

    { /* annotations not assigned to a tracklet or object */
        Group oAnno = clearOrCreateGroup(annotationsGroup, "other_annotations", otherAnnotations.size());

        for (std::shared_ptr<Annotation> a : otherAnnotations.keys()) {
            saveAnnotation(oAnno, a);
            MessageRelay::emitIncreaseDetail();
        }
    }

    return true;
}

}
