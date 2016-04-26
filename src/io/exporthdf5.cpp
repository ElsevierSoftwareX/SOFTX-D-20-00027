#include "exporthdf5.h"

#include <list>
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
#include "exceptions/ctdependencyexception.h"
#include "exceptions/ctunimplementedexception.h"
#include "provider/messagerelay.h"

namespace CellTracker {
using namespace H5;

/*!
 * \brief saves a given CellTracker::Project to a file
 * \param project the Project to save
 * \param filename the filename of the file to save to
 * \return true if saving was successfull, false otherwise
 *
 * The saving is done in different phases:
 * - CellTracker::ExportHDF5::saveTracklets
 * - CellTracker::ExportHDF5::saveEvents
 * - CellTracker::ExportHDF5::saveAnnotations
 */
bool ExportHDF5::save(std::shared_ptr<Project> project, QString filename) {
    return save(project, filename, true, true, true, true, true, true, true);
}

//bool ExportHDF5::save(std::shared_ptr<Project> project, QString filename)
//{
//    if (project->getFileName().compare(filename) != 0) {
//        /* save to a different file than where this project was loaded from */
//        QFile::copy(project->getFileName(), filename);
//    }

//    try {
//        H5File file(filename.toStdString().c_str(), H5F_ACC_RDWR, H5P_FILE_CREATE);

//        /* for a description see ImportHDF5::load */
//        struct phase {
//            bool (*functionPrt)(H5::H5File, std::shared_ptr<Project>);
//            std::string name;
//        };

//        std::list<phase> phases = {
//            {saveTracklets,   "tracklets"},
//            {saveEvents,      "events"},
//            {saveAnnotations, "annotations"}
//        };

//        MessageRelay::emitUpdateOverallName("Exporting to HDF5");
//        MessageRelay::emitUpdateOverallMax(phases.size());

//        qDebug() << "Saving to HDF5";
//        for (phase p : phases) {
//            std::string text = "Saving " + p.name;
//            qDebug() << text.c_str();
//            MessageRelay::emitUpdateDetailName(QString::fromStdString(text));
//            if (!p.functionPrt(file, project))
//                throw CTExportException(text + " failed");
//            MessageRelay::emitIncreaseOverall();
//        }

//        project->setFileName(filename);
//        qDebug() << "Finished";
//    } catch (FileIException &e) {
//        throw CTExportException("Saving the HDF5 file failed: " + e.getDetailMsg());
//    }

//    return true;
//}

bool ExportHDF5::sanityCheckOptions(std::shared_ptr<Project> proj, QString filename, bool sAnnotations, bool sAutoTracklets, bool sEvents, bool sImages, bool sInfo, bool sObjects, bool sTracklets) {
    if (proj->getFileName() == "")
        throw CTDependencyException("a project has to be loaded before saving it");
    if (QFile(filename).exists())
        throw CTDependencyException("saving to the same file is currently not supported");
    if (sAnnotations) {
        /*! \todo: check if tracklet/object annotations */
    }
    if (sAutoTracklets && !sObjects)
        throw CTDependencyException("when saving autotracklets, objects have to be saved, too");
    if (sEvents) {}  /* all good, only writes the /events folder */
    if (sImages) {}  /* all good? */
    if (sInfo) {}    /* all good? */
    if (sObjects) {} /* all good? */
    if (sTracklets && !sObjects)
        throw CTDependencyException("when saving tracklets, objects have to be saved, too");

    return true;
}

bool ExportHDF5::save(std::shared_ptr<Project> project, QString filename, bool sAnnotations, bool sAutoTracklets, bool sEvents, bool sImages, bool sInfo, bool sObjects, bool sTracklets)
{
    /* sanity check options */
    sanityCheckOptions(project, filename, sAnnotations, sAutoTracklets, sEvents, sImages, sInfo, sObjects, sTracklets);

    try {
        H5File file(filename.toStdString().c_str(), H5F_ACC_RDWR|H5F_ACC_CREAT, H5P_FILE_CREATE);

        /* for a description see ImportHDF5::load */
        struct phase {
            bool (*functionPrt)(H5::H5File, std::shared_ptr<Project>);
            std::string name;
        };

        std::list<phase> phases;
        if (sInfo)          phases.push_back({saveInfo,          "info"});
        if (sImages)        phases.push_back({saveImages,        "images"});
        if (sObjects)       phases.push_back({saveObjects,       "objects"});
        if (sAutoTracklets) phases.push_back({saveAutoTracklets, "autotracklets"});
        if (sTracklets)     phases.push_back({saveTracklets,     "tracklets"});
        if (sEvents)        phases.push_back({saveEvents,        "events"});
        if (sAnnotations)   phases.push_back({saveAnnotations,   "annotations"});

        MessageRelay::emitUpdateOverallName("Exporting to HDF5");
        MessageRelay::emitUpdateOverallMax(phases.size());

        qDebug() << "Saving to HDF5";
        for (phase p : phases) {
            std::string text = "Saving " + p.name;
            qDebug() << text.c_str();
            MessageRelay::emitUpdateDetailName(QString::fromStdString(text));
            if (!p.functionPrt(file, project))
                throw CTExportException(text + " failed");
            MessageRelay::emitIncreaseOverall();
        }

        project->setFileName(filename);
        qDebug() << "Finished";
    } catch (FileIException &e) {
        throw CTExportException("Saving the HDF5 file failed: " + e.getDetailMsg());
    }

    return true;
}

bool ExportHDF5::saveObjects(H5File file, std::shared_ptr<Project> proj) {
    H5File oldFile(proj->getFileName().toStdString().c_str(), H5F_ACC_RDWR, H5P_FILE_CREATE);
//    H5Ocopy(oldFile.getId(), "objects", file.getId(), "objects", H5P_OBJECT_COPY, H5P_LINK_CREATE);
    std::shared_ptr<Movie> mov = proj->getMovie();

    /*! \todo clearOrCreateGroup() */
    Group objectsGroup = file.createGroup("objects", 5); /* frame_rate, frames, nframes, nslices, slicing_shape */
    Group oldObjectsGroup = oldFile.openGroup("objects");

    /*! \todo is a H5G_LINK in original */
    if (groupExists(file, "/images")) { /* images don't neccessarily have to be saved, so we can't always link */
        linkOrOverwriteLink(H5L_TYPE_SOFT, objectsGroup, "/images/frame_rate", "frame_rate");
        linkOrOverwriteLink(H5L_TYPE_SOFT, objectsGroup, "/images/nframes", "nframes");
        linkOrOverwriteLink(H5L_TYPE_SOFT, objectsGroup, "/images/nslices", "nslices");
        linkOrOverwriteLink(H5L_TYPE_SOFT, objectsGroup, "/images/slicing_shape", "slicing_shape");
    } else {
        shallowCopy(oldObjectsGroup, "frame_rate", objectsGroup);
        shallowCopy(oldObjectsGroup, "nframes", objectsGroup);
        shallowCopy(oldObjectsGroup, "nslices", objectsGroup);
        shallowCopy(oldObjectsGroup, "slicing_shape", objectsGroup);
    }

    /*! \todo clearOrCreateGroup() */
    Group framesGroup = objectsGroup.createGroup("frames", mov->getFrames().count());
    Group oldFramesGroup = oldObjectsGroup.openGroup("frames");

    for (std::shared_ptr<Frame> frame : mov->getFrames()) {
        uint32_t frameId = frame->getID();

        Group frameGroup = framesGroup.createGroup(std::to_string(frameId), 2); /* frame_id, slices */
        Group oldFrameGroup = oldFramesGroup.openGroup(std::to_string(frameId));

        shallowCopy(oldFrameGroup, "frame_id", frameGroup);

        /*! \todo clearOrCreateGroup() */
        Group slicesGroup = frameGroup.createGroup("slices", frame->getSlices().count());
        Group oldSlicesGroup = oldFrameGroup.openGroup("slices");

        for (std::shared_ptr<Slice> slice : frame->getSlices()) {
            uint32_t sliceId = slice->getSliceId();

            /*! \todo clearOrCreateGroup() */
            Group sliceGroup = slicesGroup.createGroup(std::to_string(sliceId), 4); /* channels, dimensions, nchannels, slice_id */
            Group oldSliceGroup = oldSlicesGroup.openGroup(std::to_string(sliceId));

            std::string path = "/images/frames/" + std::to_string(slice->getFrameId()) +
                               "/slices/" + std::to_string(slice->getSliceId());
            if (groupExists(file, path.c_str())) {
                linkOrOverwriteLink(H5L_TYPE_SOFT, sliceGroup, path + "/dimensions", "dimensions");
                linkOrOverwriteLink(H5L_TYPE_SOFT, sliceGroup, path + "/nchannels", "nchannels");
            } else {
                shallowCopy(oldSliceGroup, "dimensions", sliceGroup);
                shallowCopy(oldSliceGroup, "nchannels", sliceGroup);
            }
            shallowCopy(oldSliceGroup, "slice_id", sliceGroup);

            /*! \todo clearOrCreateGroup() */
            Group channelsGroup = sliceGroup.createGroup("channels", slice->getChannels().count());
            Group oldChannelsGroup = oldSliceGroup.openGroup("channels");

            for (std::shared_ptr<Channel> channel : slice->getChannels()) {
                uint32_t channelId = channel->getChanId();

                Group channelGroup = channelsGroup.createGroup(std::to_string(channelId), 2); /* channel_id, objects */
                Group oldChannelGroup = oldChannelsGroup.openGroup(std::to_string(channelId));

                shallowCopy(oldChannelGroup, "channel_id", channelGroup);

                /*! \todo clearOrCreateGroup() */
                Group objectsGroup = channelGroup.createGroup("objects", channel->getObjects().count());
                Group oldObjectsGroup = oldChannelGroup.openGroup("objects");

                for (std::shared_ptr<Object> object : channel->getObjects()) {
                    uint32_t objectId = object->getId();

                    Group objectGroup = objectsGroup.createGroup(std::to_string(objectId), 8); /* bounding_box, centroid, channel_id, frame_id, object_id, outline, packed_mask, slice_id */
                    if (groupExists(oldObjectsGroup, std::to_string(objectId).c_str())) {
                        Group oldObjectGroup = oldObjectsGroup.openGroup(std::to_string(objectId));

                        shallowCopy(oldObjectGroup, "bounding_box", objectGroup);
                        shallowCopy(oldObjectGroup, "centroid", objectGroup);
                        shallowCopy(oldObjectGroup, "channel_id", objectGroup);
                        shallowCopy(oldObjectGroup, "frame_id", objectGroup);
                        shallowCopy(oldObjectGroup, "object_id", objectGroup);
                        shallowCopy(oldObjectGroup, "outline", objectGroup);
                        shallowCopy(oldObjectGroup, "packed_mask", objectGroup);
                        shallowCopy(oldObjectGroup, "slice_id", objectGroup);
                    } else {
                        using CSI = Project::CoordinateSystemInfo;
                        using CSD = CSI::CoordinateSystemData;
                        using CST = CSI::CoordinateSystemType;
                        std::shared_ptr<CSI> csi = proj->getCoordinateSystemInfo();
                        CSD csd = csi->getCoordinateSystemData();
                        {   /* bounding box */
                            std::shared_ptr<QRect> bb = object->getBoundingBox();
                            int x1, y1, x2, y2;
                            bb->getCoords(&x1, &y1, &x2, &y2);
                            switch (csi->getCoordinateSystemType()) {
                            case CST::CST_CARTESIAN:
                                y1 = csd.imageWidth - y1;
                                y2 = csd.imageWidth - y2;
                                break;
                            case CST::CST_QTIMAGE:
                                break;
                            default:
                                break;
                            }
                            uint16_t bounding_box[2][2] = {
                                { static_cast<uint16_t>(x1), static_cast<uint16_t>(y1) },
                                { static_cast<uint16_t>(x2), static_cast<uint16_t>(y2) } };
                            hsize_t dims[] = {2, 2};

                            writeMultipleValues<uint16_t>(*bounding_box, objectGroup, "bounding_box", PredType::NATIVE_UINT16, 2, dims);
                        } { /* centroid */
                            std::shared_ptr<QPoint> c = object->getCentroid();
                            uint16_t x, y;
                            x = c->x();
                            switch (csi->getCoordinateSystemType()) {
                            case CST::CST_CARTESIAN:
                                y = csd.imageWidth - c->y();
                                break;
                            case CST::CST_QTIMAGE:
                                y = c->y();
                                break;
                            }
                            uint16_t centroid[2] = { static_cast<uint16_t>(x), static_cast<uint16_t>(y) };
                            hsize_t dims[] = { 1, 2 };
                            writeMultipleValues<uint16_t>(centroid, objectGroup, "centroid", PredType::NATIVE_UINT16, 2, dims);
                        } { /* ids */
                            uint16_t channelId = object->getChannelId();
                            uint32_t frameId = object->getFrameId();
                            uint32_t objectId = object->getId();
                            uint32_t sliceId = object->getSliceId();

                            writeSingleValue<uint16_t>(channelId, objectGroup, "channel_id", PredType::NATIVE_UINT16);
                            writeSingleValue<uint32_t>(frameId, objectGroup, "frame_id", PredType::NATIVE_UINT32);
                            writeSingleValue<uint32_t>(objectId, objectGroup, "object_id", PredType::NATIVE_UINT32);
                            writeSingleValue<uint32_t>(sliceId, objectGroup, "slice_id", PredType::NATIVE_UINT32);
                        } { /* outline */
                            std::vector<uint32_t> ps;
                            int s = object->getOutline()->size();
                            if (object->getOutline()->isClosed()) s--;
                            for (int i = 0; i < s; i++) {
                                QPointF p = object->getOutline()->at(i);

                                uint32_t x, y;
                                x = p.x();
                                switch (csi->getCoordinateSystemType()) {
                                case CST::CST_CARTESIAN:
                                    y = csd.imageWidth - p.y();
                                    break;
                                case CST::CST_QTIMAGE:
                                    y = p.y();
                                    break;
                                }
                                ps.push_back(static_cast<uint32_t>(x));
                                ps.push_back(static_cast<uint32_t>(y));
                            }
                            hsize_t dims[] = { ps.size()/2, 2 };
                            writeMultipleValues<uint32_t>(&ps.front(), objectGroup, "outline", PredType::NATIVE_UINT32, 2, dims);
                        } { /* packed_mask */
                            /*! \todo packed_mask is unimplemented */
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool ExportHDF5::saveInfo(H5File file, std::shared_ptr<Project> proj) {
    H5File oldFile(proj->getFileName().toStdString().c_str(), H5F_ACC_RDWR, H5P_FILE_CREATE);

    if (groupExists(file, "info"))
        return true;

    hid_t ocpypl_id = H5P_OBJECT_COPY_DEFAULT;
    hid_t lcpl_id = H5P_LINK_CREATE_DEFAULT;

    /* make a deep copy */
    if (H5Ocopy(oldFile.getId(), "info", file.getId(), "info", ocpypl_id, lcpl_id) < 0)
        return false;

    if (H5Ocopy(oldFile.getId(), "coordinate_format", file.getId(), "coordinate_format", ocpypl_id, lcpl_id) < 0)
        return false;

    if (H5Ocopy(oldFile.getId(), "data_format_version", file.getId(), "data_format_version", ocpypl_id, lcpl_id) < 0)
        return false;

    return true;
}
bool ExportHDF5::saveImages(H5File file, std::shared_ptr<Project> proj) {
    if (!proj)
        return false;

    std::shared_ptr<Movie> mov = proj->getMovie();

    if (groupExists(file, "images")) /* images are already there, nothing to do */
        return true;

    H5File oldFile(proj->getFileName().toStdString().c_str(), H5F_ACC_RDWR, H5P_FILE_CREATE);

    Group images = file.createGroup("images", 5); /* frame_rate, frames, nframes, nslices, slicing_shape */
    Group oldImages = oldFile.openGroup("images");

    shallowCopy(oldImages, "frame_rate", images);
    shallowCopy(oldImages, "nframes", images);
    shallowCopy(oldImages, "nslices", images);
    shallowCopy(oldImages, "slicing_shape", images);

    if (groupExists(images, "frames"))
        return false; /* should not exist */

    Group framesGroup = images.createGroup("frames", mov->getFrames().count());
    Group oldFramesGroup = oldImages.openGroup("frames");

    for (uint32_t frameId : mov->getFrames().keys()) {
        std::shared_ptr<Frame> frame = mov->getFrame(frameId);
        Group frameGroup = framesGroup.createGroup(std::to_string(frameId), 2); /* frame_id, slices */
        Group oldFrameGroup = oldFramesGroup.openGroup(std::to_string(frameId));

        shallowCopy(oldFrameGroup, "frame_id", frameGroup);

        Group slicesGroup = frameGroup.createGroup("slices", frame->getSlices().count());
        Group oldSlicesGroup = oldFrameGroup.openGroup("slices");

        for (std::shared_ptr<Slice> slice : frame->getSlices()) {
            uint32_t sliceId = slice->getSliceId();
            Group sliceGroup = slicesGroup.createGroup(std::to_string(sliceId), 4); /* channels, dimensions, nchannels, slice_id */
            Group oldSliceGroup = oldSlicesGroup.openGroup(std::to_string(sliceId));

            shallowCopy(oldSliceGroup, "dimensions", sliceGroup);
            shallowCopy(oldSliceGroup, "nchannels", sliceGroup);
            shallowCopy(oldSliceGroup, "slice_id", sliceGroup);

            Group channelsGroup = sliceGroup.createGroup("channels", slice->getChannels().count());
            Group oldChannelsGroup = oldSliceGroup.openGroup("channels");

            for (std::shared_ptr<Channel> channel : slice->getChannels()) {
                uint32_t channelId = channel->getChanId();

                shallowCopy(oldChannelsGroup, std::to_string(channelId).c_str(), channelsGroup);
            }
        }
    }
    return true;
}
bool ExportHDF5::saveAutoTracklets(H5File file, std::shared_ptr<Project> proj) {
    Group autoTrackletsGroup = clearOrCreateGroup(file, "autotracklets", proj->getAutoTracklets().count());

    for (std::shared_ptr<AutoTracklet> autotracklet : proj->getAutoTracklets()) {
        uint32_t autotrackletId = autotracklet->getID();
        uint32_t start = autotracklet->getStart();
        uint32_t end = autotracklet->getEnd();

        /*! \todo clearOrCreateGroup() */
        Group autoTrackletGroup = autoTrackletsGroup.createGroup(std::to_string(autotrackletId), 3); /* id, start, end, objects */
        /* {next,previous}_{,event} are written in saveEvents */

        writeSingleValue<uint32_t>(autotrackletId, autoTrackletGroup, "autotracklet_id", PredType::NATIVE_UINT32);
        writeSingleValue<uint32_t>(start, autoTrackletGroup, "start", PredType::NATIVE_UINT32);
        writeSingleValue<uint32_t>(end, autoTrackletGroup, "end", PredType::NATIVE_UINT32);

        /*! \todo clearOrCreateGroup() */
        Group objectsGroup = autoTrackletGroup.createGroup("objects", autotracklet->getComponents().count());
        for (std::shared_ptr<Object> object : autotracklet->getComponents()) {
            uint32_t frameId = object->getFrameId();
            uint32_t sliceId = object->getSliceId();
            uint32_t channId = object->getChannelId();
            uint32_t objectId = object->getId();

            std::string objectPath = "/objects/frames/" + std::to_string(frameId) +
                                     "/slices/" + std::to_string(sliceId) +
                                     "/channels/" + std::to_string(channId) +
                                     "/objects/" + std::to_string(objectId);

            linkOrOverwriteLink(H5L_TYPE_SOFT, objectsGroup, objectPath, std::to_string(frameId));
        }
    }

    return true;
}



/*!
 * \brief this saves the Events in the Tracklets to a HDF5 file
 * \param file the file to save to
 * \param proj the Project to save
 * \return true if saving was successful, false otherwise
 */
bool ExportHDF5::saveEvents(H5File file, std::shared_ptr<Project> proj) {
    QList<std::shared_ptr<Tracklet>> ts = proj->getGenealogy()->getTracklets()->values();

    if (!groupExists(file, "events")) {
        Group eventsGroup = file.createGroup("events");
        std::vector<std::pair<std::string,std::string>> names =
        { {"cell_death",    "The cell is identified as dead."},
          {"cell_division", "The cell divides in 2 or more other cells."},
          {"cell_lost",     "The cell is lost during the tracking process."},
          {"cell_merge",    "Two or more cells are merged into an object."},
          {"cell_unmerge",  "Two or more cells unmerge from an object."},
          {"end_of_movie",  "The cell track ends at the end of the movie."} };
        for (unsigned int i = 0; i < names.size(); i++) {
            StrType st(PredType::C_S1, H5T_VARIABLE);
            std::pair<std::string,std::string> p = names.at(i);

            Group evGroup = eventsGroup.createGroup(p.first, 3);

            writeSingleValue<const char *>(p.second.c_str(), evGroup, "description", st);
            writeSingleValue<unsigned int>(i, evGroup, "event_id", PredType::NATIVE_UINT16);
            writeSingleValue<const char *>(p.first.c_str(), evGroup, "name", st);
        }
    }

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
                nextEvPath = "/events/cell_death";
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_DIVISION: {
                nextEvPath = "/events/cell_division";
                std::shared_ptr<TrackEventDivision<Tracklet>> ted = std::static_pointer_cast<TrackEventDivision<Tracklet>>(te);
                for (std::weak_ptr<Tracklet> t : *ted->getNext())
                    next.push_back(t.lock());
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_LOST: {
                nextEvPath = "/events/cell_lost";
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_ENDOFMOVIE: {
                nextEvPath = "/events/end_of_movie";
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_MERGE: {
                nextEvPath = "/events/cell_merge";
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_UNMERGE: {
                nextEvPath = "/events/cell_unmerge";
                std::shared_ptr<TrackEventUnmerge<Tracklet>> teu = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(te);
                for (std::weak_ptr<Tracklet> t : *teu->getNext())
                    next.push_back(t.lock());
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
                prev.push_back(ted->getPrev().lock());
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_LOST: {
                qDebug() << "TrackEventDead should never be previous";
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_ENDOFMOVIE: {
                qDebug() << "TrackEventEndOfMovie should never be previous";
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_MERGE: {
                prevEvPath = "/events/cell_merge";
                std::shared_ptr<TrackEventMerge<Tracklet>> tem = std::static_pointer_cast<TrackEventMerge<Tracklet>>(te);
                for (std::weak_ptr<Tracklet> t : *tem->getPrev())
                    prev.push_back(t.lock());
                break; }
            case TrackEvent<Tracklet>::EVENT_TYPE_UNMERGE: {
                prevEvPath = "/events/cell_unmerge";
                std::shared_ptr<TrackEventUnmerge<Tracklet>> teu = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(te);
                prev.push_back(teu->getPrev().lock());
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

/*!
 * \brief when saving a Tracklet, this creates the links in the /tracklets/\<id\>/objects group
 * \param file the file to save to
 * \param grp the group of the Tracklet
 * \param t the Tracklet, whose contained Objects should be saved
 * \return true if saving was successful, false otherwise
 */
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

/*!
 * \brief this saves the next_event dataset and the next group of a Tracklet
 * \param grp the group to save to
 * \param t the Tracklet whose next-event should be saved
 * \return true on success, false otherwise
 */
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
        for (std::weak_ptr<Tracklet> nt : *ted->getNext())
            next.push_back(nt.lock());
        break; }
    case TrackEvent<Tracklet>::EVENT_TYPE_LOST:
        nextType = "cell lost";
        hasNextTracklets = false;
        break;
    case TrackEvent<Tracklet>::EVENT_TYPE_ENDOFMOVIE:
        nextType = "end of movie";
        hasNextTracklets = false;
        break;
    case TrackEvent<Tracklet>::EVENT_TYPE_MERGE: {
        nextType = "cell merge";
        hasNextTracklets = true;
        std::shared_ptr<TrackEventMerge<Tracklet>> tem = std::static_pointer_cast<TrackEventMerge<Tracklet>>(te);
        next.push_back(tem->getNext().lock());
        break; }
    case TrackEvent<Tracklet>::EVENT_TYPE_UNMERGE: {
        nextType = "cell unmerge";
        hasNextTracklets = true;
        std::shared_ptr<TrackEventUnmerge<Tracklet>> teu = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(te);
        for (std::weak_ptr<Tracklet> nt : *teu->getNext())
            next.push_back(nt.lock());
        break; }
    }

    /* link the type */
    linkOrOverwriteLink(H5L_TYPE_SOFT, grp, "/events/"+nextType, "next_event");

    if (hasNextTracklets) {
        Group nextGroup = grp.createGroup("next");
        for (std::shared_ptr<Tracklet> tr : next)
            linkOrOverwriteLink(H5L_TYPE_SOFT, nextGroup, "/tracklets/"+std::to_string(tr->getId()), std::to_string(tr->getId()));
    }
    return true;
}

/*!
 * \brief this saves the previous_event dataset and the previous group of a Tracklet
 * \param grp the group to save to
 * \param t the Tracklet whose previous-event should be saved
 * \return true on success, false otherwise
 */
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
        prev.push_back(ted->getPrev().lock());
        break; }
    case TrackEvent<Tracklet>::EVENT_TYPE_LOST:
        /* not possible */
        break;
    case TrackEvent<Tracklet>::EVENT_TYPE_ENDOFMOVIE:
        /* not possible */
        break;
    case TrackEvent<Tracklet>::EVENT_TYPE_MERGE: {
        prevType = "cell merge";
        hasPrevTracklets = true;
        std::shared_ptr<TrackEventMerge<Tracklet>> tem = std::static_pointer_cast<TrackEventMerge<Tracklet>>(te);
        for (std::weak_ptr<Tracklet> pt : *tem->getPrev())
            prev.push_back(pt.lock());
        break; }
    case TrackEvent<Tracklet>::EVENT_TYPE_UNMERGE: {
        prevType = "cell unmerge";
        hasPrevTracklets = true;
        std::shared_ptr<TrackEventUnmerge<Tracklet>> teu = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(te);
        prev.push_back(teu->getPrev().lock());
        break; }
    }

    /* link the type */
    linkOrOverwriteLink(H5L_TYPE_SOFT, grp, "/events/"+prevType, "previous_event");

    if (hasPrevTracklets) {
        Group prevGroup = grp.createGroup("previous");
        for (std::shared_ptr<Tracklet> tr : prev)
            linkOrOverwriteLink(H5L_TYPE_SOFT, prevGroup, "/tracklets/"+std::to_string(tr->getId()), std::to_string(tr->getId()));
    }
    return true;
}

/*!
 * \brief this saves the Tracklets of a Project to a HDF5 file
 * \param file the file to save to
 * \param project the Project whose Tracklets should be saved
 * \return true on success, false otherwise
 */
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

/*!
 * \brief this saves an Annotation to the /annotations group of a HDF5 file
 * \param grp the /annotations group to save to
 * \param a the Annotation to save
 * \return true on success, false otherwise
 */
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
    writeSingleValue<const char*>(a->getTitle().toStdString().c_str(), aGroup, "title", st);
    writeSingleValue<const char*>(a->getDescription().toStdString().c_str(), aGroup, "description", st);
    return true;
}

/*!
 * \brief this saves all Annotations and who they are assigned to
 * \param file the file to save to
 * \param project the Project that contains the annotations/annotatees to save
 * \return true on success, false otherwise
 */
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

    { /* save all annotation assignments */
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
