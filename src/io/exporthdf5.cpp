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

namespace CellTracker {
using namespace H5;

ExportHDF5::ExportHDF5() {}
ExportHDF5::~ExportHDF5() {}

bool ExportHDF5::save(std::shared_ptr<Project> project, QString filename)
{
    if (project->getFileName().compare(filename) != 0) {
        /* save to a different file than where this project was loaded from */
        QFile::copy(project->getFileName(), filename);
    }

    try {
        H5File file(filename.toStdString().c_str(), H5F_ACC_RDWR, H5P_FILE_CREATE);

        MessageRelay::emitUpdateOverallName("Exporting to HDF5");
        MessageRelay::emitUpdateOverallMax(3);

        qDebug() << "Saving tracklets";
        if (!saveTracklets(file, project))
            throw CTExportException("Saving the tracklets failed");
        MessageRelay::emitIncreaseOverall();

        qDebug() << "Saving events";
        if (!saveEvents(file, project))
            throw CTExportException("Saving the events failed");
        MessageRelay::emitIncreaseOverall();

        qDebug() << "Saving annotations";
        if (!saveAnnotations(file, project))
            throw CTExportException("Saving the annotations failed");
        MessageRelay::emitIncreaseOverall();

        project->setFileName(filename);
        qDebug() << "Finished";
    } catch (FileIException &e) {
        throw CTExportException("Saving the HDF5 file failed: " + e.getDetailMsg());
    }

    return true;
}

bool ExportHDF5::saveEvents(H5File file, std::shared_ptr<Project> proj) {
    QList<std::shared_ptr<Tracklet>> ts = proj->getGenealogy()->getTracklets()->values();

    for (std::shared_ptr<Tracklet> tr : ts) {
        bool hasNext = false;
        bool hasPrev = false;
        if (tr->getNext() != nullptr)
            hasNext = true;
        if (tr->getPrev() != nullptr)
            hasPrev = true;

        std::string grpPath = "/tracklets/" + std::to_string(tr->getId());
        Group tGrp = file.openGroup(grpPath);
        if (hasNext) {
            /* save next_event */
            std::string nextEvPath;
            std::list<std::shared_ptr<Tracklet>> next;

            std::shared_ptr<TrackEvent<Tracklet>> te = tr->getNext();
            switch (te->getType()) {
            case TrackEvent<Tracklet>::EVENT_TYPE_DEAD: {
                nextEvPath = "/events/cell_dead";
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_DIVISION: {
                nextEvPath = "/events/cell_division";
                std::shared_ptr<TrackEventDivision<Tracklet>> ted = std::static_pointer_cast<TrackEventDivision<Tracklet>>(te);
                for (std::shared_ptr<Tracklet> t : *ted->getNext())
                    next.push_back(t);
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_LOST: {
                nextEvPath = "/events/cell_lost";
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_MERGE: {
                nextEvPath = "/events/cell_merge";
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_UNMERGE: {
                nextEvPath = "/events/cell_unmerge";
                std::shared_ptr<TrackEventUnmerge<Tracklet>> teu = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(te);
                for (std::shared_ptr<Tracklet> t : *teu->getNext())
                    next.push_back(t);
                break; }
            }

            linkOrOverwriteLink(H5L_TYPE_SOFT, tGrp, nextEvPath, "next_event");

            /* save next */
            if (!next.empty()) {
                Group nextGroup = clearOrCreateGroup(tGrp, "next", next.size());
                int i = 0;
                for (std::shared_ptr<Tracklet> t : next) {
                    linkOrOverwriteLink(H5L_TYPE_SOFT, nextGroup, "/tracklets/" + std::to_string(t->getId()), std::to_string(i));
                    i++;
                }
            }
        }
        if (hasPrev) {
            /* save previous_event */
            std::string prevEvPath;
            std::list<std::shared_ptr<Tracklet>> prev;

            std::shared_ptr<TrackEvent<Tracklet>> te = tr->getPrev();
            switch (te->getType()) {
            case TrackEvent<Tracklet>::EVENT_TYPE_DEAD: {
                qDebug() << "TrackEventDead should never be previous";
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_DIVISION: {
                prevEvPath = "/events/cell_division";
                std::shared_ptr<TrackEventDivision<Tracklet>> ted = std::static_pointer_cast<TrackEventDivision<Tracklet>>(te);
                prev.push_back(ted->getPrev());
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_LOST: {
                qDebug() << "TrackEventDead should never be previous";
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_MERGE: {
                prevEvPath = "/events/cell_merge";
                std::shared_ptr<TrackEventMerge<Tracklet>> tem = std::static_pointer_cast<TrackEventMerge<Tracklet>>(te);
                for (std::shared_ptr<Tracklet> t : *tem->getPrev())
                    prev.push_back(t);
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_UNMERGE: {
                prevEvPath = "/events/cell_unmerge";
                std::shared_ptr<TrackEventUnmerge<Tracklet>> teu = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(te);
                prev.push_back(teu->getPrev());
                break; }
            }

            linkOrOverwriteLink(H5L_TYPE_SOFT, tGrp, prevEvPath, "previous_event");

            /* save previous */
            if (!prev.empty()) {
                Group prevGroup = clearOrCreateGroup(tGrp, "previous", prev.size());
                int i = 0;
                for (std::shared_ptr<Tracklet> t : prev) {
                    linkOrOverwriteLink(H5L_TYPE_SOFT, prevGroup, "/tracklets/" + std::to_string(t->getId()), std::to_string(i));
                    i++;
                }
            }
        }
    }
    return true;
}

bool ExportHDF5::saveTrackletsContained(H5File file, Group grp, std::shared_ptr<Tracklet> t) {
    Group containedGroup = grp.createGroup("objects");
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
                +"/objects/"+std::to_string(oId);
        if(!linkExists(file, target.c_str()))
            qDebug() << target.c_str() << "does not exist";
        linkOrOverwriteLink(H5L_TYPE_SOFT, containedGroup, target, std::to_string(fId));
    }
    return true;
}

bool ExportHDF5::saveAnnotationAssignments(H5File file, Group grp, std::shared_ptr<Annotateable> a) {
    std::shared_ptr<QList<std::shared_ptr<Annotation>>> annotations = a->getAnnotations();

    Group annotationsGroup = grp.createGroup("annotations", annotations->length());

    for (std::shared_ptr<Annotation> ai : *annotations) {
        std::string target;
        switch (ai->getType()) {
        case CellTracker::Annotateable::OBJECT_ANNOTATION:
            target = "/annotations/object_annotations/" + std::to_string(ai->getId());
            break;
        case CellTracker::Annotateable::TRACKLET_ANNOTATION:
            target = "/annotations/track_annotations/" + std::to_string(ai->getId());
            break;
        }
        if(!linkExists(file, target.c_str()))
            qDebug() << target.c_str() << "does not exist";
        linkOrOverwriteLink(H5L_TYPE_SOFT, annotationsGroup, target, std::to_string(ai->getId()));
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

bool ExportHDF5::saveTrackletsNextEvent(Group grp, std::shared_ptr<Tracklet> t) {
    std::shared_ptr<TrackEvent<Tracklet>> te = t->getNext();
    bool hasNextTracklets = false;
    std::string nextType;
    QList<std::shared_ptr<Tracklet>> next;

    switch (te->getType()) {
    case TrackEvent<Tracklet>::EVENT_TYPE_DEAD:
        nextType = "cell dead";
        hasNextTracklets = false;
        break;
    case TrackEvent<Tracklet>::EVENT_TYPE_DIVISION: {
        nextType = "cell division";
        hasNextTracklets = true;
        std::shared_ptr<TrackEventDivision<Tracklet>> ted = std::static_pointer_cast<TrackEventDivision<Tracklet>>(te);
        for (std::shared_ptr<Tracklet> nt : *ted->getNext())
            next.push_back(nt);
        break; }
    case TrackEvent<Tracklet>::EVENT_TYPE_LOST:
        nextType = "cell lost";
        hasNextTracklets = false;
        break;
    case TrackEvent<Tracklet>::EVENT_TYPE_MERGE: {
        nextType = "cell merge";
        hasNextTracklets = true;
        std::shared_ptr<TrackEventMerge<Tracklet>> tem = std::static_pointer_cast<TrackEventMerge<Tracklet>>(te);
        next.push_back(tem->getNext());
        break; }
    case TrackEvent<Tracklet>::EVENT_TYPE_UNMERGE: {
        nextType = "cell unmerge";
        hasNextTracklets = true;
        std::shared_ptr<TrackEventUnmerge<Tracklet>> teu = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(te);
        for (std::shared_ptr<Tracklet> nt : *teu->getNext())
            next.push_back(nt);
        break; }
    }

    /* link the type */
    linkOrOverwriteLink(H5L_TYPE_SOFT, grp, "/events/"+nextType, "next_event");

    if (hasNextTracklets) {
        Group nextGroup = grp.createGroup("next");
        for (std::shared_ptr<Tracklet> tr : next)
            linkOrOverwriteLink(H5L_TYPE_SOFT, nextGroup, "/tracklets/"+std::to_string(t->getId()), std::to_string(t->getId()));
    }
    return true;
}

bool ExportHDF5::saveTrackletsPreviousEvent(Group grp, std::shared_ptr<Tracklet> t) {
    std::shared_ptr<TrackEvent<Tracklet>> te = t->getPrev();
    bool hasPrevTracklets = false;
    std::string prevType;
    QList<std::shared_ptr<Tracklet>> prev;

    switch (te->getType()) {
    case TrackEvent<Tracklet>::EVENT_TYPE_DEAD:
        /* not possible */
        break;
    case TrackEvent<Tracklet>::EVENT_TYPE_DIVISION: {
        prevType = "cell division";
        hasPrevTracklets = true;
        std::shared_ptr<TrackEventDivision<Tracklet>> ted = std::static_pointer_cast<TrackEventDivision<Tracklet>>(te);
        prev.push_back(ted->getPrev());
        break; }
    case TrackEvent<Tracklet>::EVENT_TYPE_LOST:
        /* not possible */
        break;
    case TrackEvent<Tracklet>::EVENT_TYPE_MERGE: {
        prevType = "cell merge";
        hasPrevTracklets = true;
        std::shared_ptr<TrackEventMerge<Tracklet>> tem = std::static_pointer_cast<TrackEventMerge<Tracklet>>(te);
        for (std::shared_ptr<Tracklet> pt : *tem->getPrev())
            prev.push_back(pt);
        break; }
    case TrackEvent<Tracklet>::EVENT_TYPE_UNMERGE: {
        prevType = "cell unmerge";
        hasPrevTracklets = true;
        std::shared_ptr<TrackEventUnmerge<Tracklet>> teu = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(te);
        prev.push_back(teu->getPrev());
        break; }
    }

    /* link the type */
    linkOrOverwriteLink(H5L_TYPE_SOFT, grp, "/events/"+prevType, "previous_event");

    if (hasPrevTracklets) {
        Group prevGroup = grp.createGroup("previous");
        for (std::shared_ptr<Tracklet> tr : prev)
            linkOrOverwriteLink(H5L_TYPE_SOFT, prevGroup, "/tracklets/"+std::to_string(t->getId()), std::to_string(t->getId()));
    }
    return true;
}

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
        Group trackletGroup = clearOrCreateGroup(trackletsGroup, std::to_string(t->getId()).c_str(), size);

        /* write id of this tracklet, start and end */
        writeSingleValue<uint32_t>(t->getId(), trackletGroup, "tracklet_id", PredType::NATIVE_UINT32);
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
    std::string id_name;
    switch (a->getType()) {
    case Annotation::ANNOTATION_TYPE::OBJECT_ANNOTATION: id_name = "object_annotation_id"; break;
    case Annotation::ANNOTATION_TYPE::TRACKLET_ANNOTATION: id_name = "track_annotation_id" ; break;
    }
    writeSingleValue<uint32_t>(a->getId(), aGroup, id_name.c_str(), PredType::NATIVE_UINT32);
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

    QList<std::shared_ptr<Annotation>> objectAnnotations;
    QList<std::shared_ptr<Annotation>> trackAnnotations;

    for (std::shared_ptr<Annotation> annotation : *allAnnotations) {
        switch (annotation->getType()) {
        case Annotateable::OBJECT_ANNOTATION:
            objectAnnotations.push_back(annotation);
            break;
        case Annotateable::TRACKLET_ANNOTATION:
            trackAnnotations.push_back(annotation);
            break;
        }
    }

    { /* object annotations */
        Group oAnno = clearOrCreateGroup(annotationsGroup, "object_annotations", objectAnnotations.size());
        for (std::shared_ptr<Annotation> a : objectAnnotations) {
            saveAnnotation(oAnno, a);
            MessageRelay::emitIncreaseDetail();
        }
    }

    { /* track annotations */
        Group tAnno = clearOrCreateGroup(annotationsGroup, "track_annotations", trackAnnotations.size());
        for (std::shared_ptr<Annotation> a : trackAnnotations) {
            saveAnnotation(tAnno, a);
            MessageRelay::emitIncreaseDetail();
        }
    }

    { /* save all annotation instances */
        for (std::shared_ptr<Annotateable> a : *allAnnotated) {
            std::shared_ptr<QList<std::shared_ptr<Annotation>>> annotations = a->getAnnotations();
            std::string annotationBase, targetString;
            switch (annotations->first()->getType()) {
            case CellTracker::Annotateable::OBJECT_ANNOTATION: {
                std::shared_ptr<Object> object = std::static_pointer_cast<Object>(a);
                targetString = "/objects/frames/" + std::to_string(object->getFrameId())
                        + "/slices/" + std::to_string(object->getSliceId())
                        + "/channels/" + std::to_string(object->getChannelId())
                        + "/objects/" + std::to_string(object->getId())
                        + "/annotations";
                annotationBase = "/annotations/object_annotations/";
                break; }
            case CellTracker::Annotateable::TRACKLET_ANNOTATION: {
                std::shared_ptr<Tracklet> tracklet = std::static_pointer_cast<Tracklet>(a);
                targetString = "/tracklets/" + std::to_string(tracklet->getId()) + "/annotations";
                annotationBase = "/annotations/track_annotations/";
                break; }
            }
            /* clear or create the group in which the links to the annotations will be stored */
            Group targetGroup = clearOrCreateGroup(file, targetString.c_str(), a->getAnnotations()->size());
            int i = 0;
            for (std::shared_ptr<Annotation> annotation : *a->getAnnotations()) {
                std::string fullAnnotationPath = annotationBase + std::to_string(annotation->getId());
                linkOrOverwriteLink(H5L_TYPE_SOFT, targetGroup, fullAnnotationPath, std::to_string(i++));
            }
        }
    }

    return true;
}

}
