#include "exporthdf5.h"

#include <set>
#include <H5Cpp.h>
#include <QDebug>

#include "tracked/trackevent.h"
#include "tracked/trackeventdead.h"
#include "tracked/trackeventdivision.h"
#include "tracked/trackeventlost.h"
#include "tracked/trackeventmerge.h"
#include "tracked/trackeventunmerge.h"
#include "hdf5_aux.h"
#include "exceptions/ctexportexception.h"
#include "provider/messagerelay.h"


namespace CellTracker {
using namespace H5;

ExportHDF5::ExportHDF5() { }
ExportHDF5::~ExportHDF5() { }

bool ExportHDF5::save(std::shared_ptr<Project> project, QString filename)
{

    try {
        H5File file(filename.toStdString().c_str(), H5F_ACC_RDWR, H5P_FILE_CREATE);

        MessageRelay::emitUpdateOverallName("Exporting to HDF5");
        MessageRelay::emitUpdateOverallMax(3);

        qDebug() << "Saving tracklets";
        if (!saveTracklets(file, project))
            throw CTExportException("Saving the tracklets failed");
        MessageRelay::emitIncreaseOverall();

        qDebug() << "Saving annotations";
        if (!saveAnnotations(file, project))
            throw CTExportException("Saving the annotations failed");
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

bool ExportHDF5::saveTracklets(H5File file, std::shared_ptr<Project> project)
{
    std::shared_ptr<QHash<int,std::shared_ptr<Tracklet>>> tracklets = project->getGenealogy()->getTracklets();
    Group trackletsGroup = openOrCreateGroup(file, "/tracklets", tracklets->size());

    MessageRelay::emitUpdateDetailName("Saving tracklets");
    MessageRelay::emitUpdateDetailMax(tracklets->size());

    for (std::shared_ptr<Tracklet> t: *tracklets) {
        /* we don't want objects of old tracklets lying around */
        Group trackletGroup = clearOrCreateGroup(trackletsGroup, std::to_string(t->getID()).c_str());

        /* write id of this tracklet, start and end */
        writeSingleValue<uint32_t>(t->getID(), trackletGroup, "tracklet_id", PredType::NATIVE_UINT32);
        writeSingleValue<uint32_t>(t->getStart().first->getID(), trackletGroup, "start", PredType::NATIVE_UINT32);
        writeSingleValue<uint32_t>(t->getEnd().first->getID(), trackletGroup, "end", PredType::NATIVE_UINT32);

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
            linkOrOverwriteLink(H5L_TYPE_SOFT, trackletGroup, target, std::to_string(fId));
        }
        MessageRelay::emitIncreaseDetail();
    }
    return true;
}

bool ExportHDF5::saveAnnotations(H5File file, std::shared_ptr<Project> project)
{
    std::shared_ptr<QList<std::shared_ptr<Annotateable>>> allAnnotated = project->getGenealogy()->getAnnotated();
    std::shared_ptr<QList<std::shared_ptr<Annotation>>> allAnnotations = project->getGenealogy()->getAnnotations();
    Group annotationsGroup = file.openGroup("/annotations");

    MessageRelay::emitUpdateDetailName("Saving annotations");
    MessageRelay::emitUpdateDetailMax(allAnnotations->size());

    QMultiMap<std::shared_ptr<Annotation>,std::shared_ptr<Annotateable>> objectAnnotations;
    QMultiMap<std::shared_ptr<Annotation>,std::shared_ptr<Annotateable>> trackAnnotations;

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

    /* object annotations */
    {
        Group oAnno = clearOrCreateGroup(annotationsGroup, "object_annotations", objectAnnotations.size());

        int i = 0;
        for (std::shared_ptr<Annotation> a : objectAnnotations.keys()) {
            StrType st(PredType::C_S1, H5T_VARIABLE);
            Group aGroup = oAnno.createGroup(std::to_string(i), 2);
            writeSingleValue<uint32_t>(a->getId(), aGroup, "id", PredType::NATIVE_UINT32);
            writeSingleValue<std::string>(a->getTitle().toStdString().c_str(), aGroup, "title", st);
            writeSingleValue<std::string>(a->getDescription().toStdString().c_str(), aGroup, "description", st);

            for (std::shared_ptr<Annotateable> annotated : objectAnnotations.values(a)){
                std::shared_ptr<Object> o = std::static_pointer_cast<Object>(annotated);

                std::string object = "objects/frames/" + std::to_string(o->getFrameId())
                        + "/slices/" + std::to_string(o->getSliceId())
                        + "/channels/" + std::to_string(o->getChannelId())
                        +"/" + std::to_string(o->getId());
                /*! \todo check if successful */
                Group oGrp = file.openGroup(object);
                Group aGrp = openOrCreateGroup(oGrp, "annotations", annotated->getAnnotations()->size());
                linkOrOverwriteLink(H5L_TYPE_SOFT, aGrp, "/annotations/object_annotations/" + std::to_string(i), std::to_string(i));
            }
            i++;
            MessageRelay::emitIncreaseDetail();
        }
    }

    /* track annotations */
    {
        Group tAnno = clearOrCreateGroup(annotationsGroup, "track_annotations", trackAnnotations.size());

        int i = 0;
        for (std::shared_ptr<Annotation> a : trackAnnotations.keys()) {
            StrType st(PredType::C_S1, H5T_VARIABLE);
            Group aGroup = tAnno.createGroup(std::to_string(i), 2);
            writeSingleValue<uint32_t>(a->getId(), aGroup, "id", PredType::NATIVE_UINT32);
            writeSingleValue<std::string>(a->getTitle().toStdString().c_str(), aGroup, "title", st);
            writeSingleValue<std::string>(a->getDescription().toStdString().c_str(), aGroup, "description", st);

            for (std::shared_ptr<Annotateable> annotated : trackAnnotations.values(a)){
                std::shared_ptr<Tracklet> t = std::static_pointer_cast<Tracklet>(annotated);

                std::string tracklet = "tracklets/"
                    + std::to_string(t->getID())
                    + "/";
                /*! \todo check if successful */
                Group tGrp = file.openGroup(tracklet);
                Group aGrp = openOrCreateGroup(tGrp, "annotations");
                linkOrOverwriteLink(H5L_TYPE_SOFT, aGrp, "/annotations/track_annotations/" + std::to_string(i), std::to_string(i));
            }
            i++;
            MessageRelay::emitIncreaseDetail();
        }
    }
    return true;
}

}
