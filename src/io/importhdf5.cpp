/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2017, 2016, 2015 Konstantin Thierbach, Sebastian Wagner
 *
 * Celltracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Celltracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Celltracker.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "importhdf5.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <tuple>
#include <utility>

#include <QColor>
#include <QDateTime>
#include <QDebug>
#include <QList>
#include <QLocale>
#include <QPolygonF>
#include <QPoint>
#include <QRect>

#include "hdf5_aux.h"
#include "tracked/trackeventdead.hpp"
#include "tracked/trackeventdivision.hpp"
#include "tracked/trackeventendofmovie.hpp"
#include "tracked/trackeventlost.hpp"
#include "tracked/trackeventmerge.hpp"
#include "tracked/trackeventunmerge.hpp"
#include "exceptions/ctimportexception.h"
#include "exceptions/ctformatexception.h"
#include "exceptions/ctmissingelementexception.h"
#include "provider/guistate.h"
#include "provider/messagerelay.h"

namespace CellTracker {
using namespace H5;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
static std::shared_ptr<Project> currentProject;
static QList<std::shared_ptr<Object>> annotatedObjects;
static QList<std::shared_ptr<Tracklet>> annotatedTracklets;
#pragma clang diagnostic pop

/*!
 * \brief loads a Project from a HDF5 file
 * \param fileName the filename of the HDF5 file
 * \return a std::shared_ptr<Project> to the loaded Project
 * \throw CTImportException if any of the phases fails
 *
 * Loading a project is done in different phases, currently:
 *   - CellTracker::ImportHDF5::loadInfo
 *   - CellTracker::ImportHDF5::loadEvents
 *   - CellTracker::ImportHDF5::loadAnnotations
 *   - CellTracker::ImportHDF5::loadObjects
 *   - CellTracker::ImportHDF5::loadAutoTracklets
 *   - CellTracker::ImportHDF5::loadTracklets
 *   - CellTracker::ImportHDF5::loadEventInstances
 *   - CellTracker::ImportHDF5::loadAnnotationAssignments
 *
 * Images are loaded seperately by invoking CellTracker::ImportHDF5::requestImage.
 */
std::shared_ptr<Project> ImportHDF5::load(QString fileName)
{
    std::shared_ptr<Project> proj;

    try {
        if (!H5File::isHdf5(fileName.toStdString().c_str()))
            return proj;

        H5File file(fileName.toStdString().c_str(), H5F_ACC_RDONLY);

        /* If you want to add new phases, do it here.
         *
         * This is a list of pairs of function pointers and strings, describing the phases of the import.
         *   * The function pointer has to return bool and take arguments H5::H5File and std::shared_ptr<Project> and
         *     all functions that are pointed to by function pointers in the list are called.
         *   * The string is used to describe what is going on
         */
        struct phase {
            bool (*functionPtr)(H5::H5File, std::shared_ptr<Project>);  /* function pointer to the phase */
            std::string name;                                           /* name of the phase */
        };

        std::list<phase> phases = {
                {loadInfo,                  "project information"},
                {loadEvents,                "events"},
                {loadAnnotations,           "annotations"},
                {loadObjects,               "objects"},
                {loadAutoTracklets,         "autotracklets"},
                {loadTracklets,             "tracklets"},
                {loadEventInstances,        "mother-daughter relations"},
                {loadAnnotationAssignments, "annotation assignments"}
        };

        MessageRelay::emitUpdateOverallName("Importing from HDF5");
        MessageRelay::emitUpdateOverallMax(static_cast<int>(phases.size()));

        proj = setupEmptyProject();
        proj->setFileName(fileName);
        currentProject = proj;

        qDebug() << "Importing from HDF5";
        for (phase p : phases) {
            std::string text = "Loading " + p.name;
            qDebug() << text.c_str();
            MessageRelay::emitUpdateDetailName(QString::fromStdString(text));
            if (!p.functionPtr(file, proj))
                throw CTImportException (text + " failed");
            MessageRelay::emitIncreaseOverall();
        }

        qDebug() << "Finished";
        currentProject = nullptr;
        annotatedObjects.clear();
        annotatedTracklets.clear();
    } catch (H5::FileIException &e) {
        throw CTImportException ("Opening the file " + fileName.toStdString() + " failed: " + e.getDetailMsg());
    }
    std::shared_ptr<Project> ret = proj;
    proj.reset();

    return ret;
}


/*!
 * \brief loads information about the project from a given HDF5 file
 * \param file the file, that is read from
 * \param proj the std::shared_ptr<Project> into which the information is loaded
 * \return true, if successfull, false otherwise
 *
 * Currently only reads the CoordinateSystemInfo that is used to decide whether
 * image data is in a Cartesian or in an Image-CoordinateSystem
 */
bool ImportHDF5::loadInfo (H5File file, std::shared_ptr<Project> proj) {
    try {
        MessageRelay::emitUpdateDetailMax(3);

        DataSet coordinate_format = file.openDataSet("/coordinate_format");
        {
            std::string cf;
            DataType dt = coordinate_format.getDataType();
            coordinate_format.read(cf,dt);

            auto csi = std::make_shared<Project::CoordinateSystemInfo>();

            if (cf.compare("Cartesian") == 0) {
                csi->setCoordinateSystemType(Project::CoordinateSystemInfo::CoordinateSystemType::CST_CARTESIAN);
                /* now get the dimensions */
                Group testSlice = file.openGroup("images/frames/0/slices/0");

                auto ret = readMultipleValues<uint32_t>(testSlice, "dimensions");
                uint32_t *dimensions = std::get<0>(ret);
                hsize_t *dims = std::get<1>(ret);
                int rank = std::get<2>(ret);

                if (rank != 1)
                    throw CTFormatException("hyperdimensional images?");
                if (dims[0] != 2)
                    throw CTFormatException("currently only two dimensional images are supported");

                Project::CoordinateSystemInfo::CoordinateSystemData csd = { dimensions[0], dimensions[1] };
                csi->setCoordinateSystemData(csd);

                delete[] std::get<0>(ret);
                delete[] std::get<1>(ret);
            } else {
                throw CTFormatException("Unsupported coordinate format "+cf);
            }

            proj->setCoordinateSystemInfo(csi);
        }

        MessageRelay::emitIncreaseDetail();

        {
            if (groupExists(file, "info")) {
                Group info = file.openGroup("info");

                if (datasetExists(info, "time_tracked")) {
                    auto ret = readMultipleValues<uint64_t>(info, "time_tracked");

                    uint64_t *data = std::get<0>(ret);
                    hsize_t *dims = std::get<1>(ret);
                    uint64_t rank = std::get<2>(ret);

                    if (rank != 2 || dims[1] != 2)
                        throw CTFormatException("time_tracked is malformed");

                    QVariantMap map;

                    for (unsigned idx = 0; idx < dims[0] * dims[1]; idx += 2) {
                        uint64_t date = data[idx];
                        uint64_t sum = data[idx+1];

                        QString ds = QDate::fromJulianDay(date).toString(Qt::ISODate);
                        QVariant sv = QVariant::fromValue<qulonglong>(sum);
                        map[ds] = sv;
                    }

                    delete[] dims;
                    delete[] data;

                    GUIState::getInstance()->setWorkedOnProject(map);
                }
            }
        }

        MessageRelay::emitIncreaseDetail();

//        {
//            std::string time;
//            DataSet timeOfConversion = info.openDataSet("timeOfConversion");
//            DataType datatype = timeOfConversion.getDataType();

//            timeOfConversion.read(time,datatype);
//            QDateTime dateTime = QDateTime::fromString(time.c_str(), "dd-MM-yyyy-hh:mm:ss");
//            proj->getInfo()->setTimeOfConversion(dateTime);
//        }
        MessageRelay::emitIncreaseDetail();
    } catch (H5::GroupIException &e) {
        throw CTFormatException ("Format mismatch while trying to read info: " + e.getDetailMsg());
    }

    return true;
}

/*!
 * \brief checks if the required events exist in /events
 * \param file the HDF5 file to check
 * \param proj (unused)
 * \return true if the events exist, false if not
 *
 * \warning This method doesn't actually load any events, but rather checks,
 * if they exist. This is due to the fact, that the possible Events are
 * hard-coded in CellTracker.
 */
bool ImportHDF5::loadEvents(H5File file, std::shared_ptr<Project> proj)
{
    Q_UNUSED(proj)
    Group eventsGroup = file.openGroup("events");
    std::list<std::string> requiredGroups = {"cell_death",
                                             "cell_division",
                                             "cell_lost",
                                             "cell_merge",
                                             "cell_unmerge",
                                             "end_of_movie"};
    for (std::string grpName : requiredGroups) {
        if (!linkExists(eventsGroup, grpName.c_str()))
            qDebug() << "Event" << grpName.c_str() << "does not exist.";
    }
    return true;
}

/*!
 * \brief Callback for iterating over track annotations
 * \param group_id callback parameter
 * \param name callback parameter
 * \param op_data callback parameter, holds a pointer to a QList of Annotation%s
 * \return callback status
 */
herr_t ImportHDF5::process_track_annotations (hid_t group_id, const char *name, void *op_data) {
    Genealogy *gen = static_cast<Genealogy*>(op_data);
    Group annotationElement = openGroup(group_id, name);
    uint32_t id = readSingleValue<uint32_t>(annotationElement, "track_annotation_id");
    std::string title = readString(annotationElement, "title");
    std::string description = readString(annotationElement, "description");

    QString t = QString::fromStdString(title);
    QString d = QString::fromStdString(description);

    auto a = std::make_shared<Annotation>(Annotation::TRACKLET_ANNOTATION, id, t, d);
    gen->addAnnotation(a);

    return 0;
}

/*!
 * \brief Callback for iterating over object annotations
 * \param group_id callback parameter
 * \param name callback parameter
 * \param op_data callback parameter, holds a pointer to a QList of Annotation%s
 * \return callback status
 */
herr_t ImportHDF5::process_object_annotations (hid_t group_id, const char *name, void *op_data) {
    Genealogy *gen = static_cast<Genealogy*>(op_data);
    Group annotationElement = openGroup(group_id, name);
    uint32_t id = readSingleValue<uint32_t>(annotationElement, "object_annotation_id");
    std::string title = readString(annotationElement,"title");
    std::string description = readString(annotationElement,"description");

    QString t = QString::fromStdString(title);
    QString d = QString::fromStdString(description);

    auto a = std::make_shared<Annotation>(Annotation::OBJECT_ANNOTATION, id, t, d);
    gen->addAnnotation(a);

    return 0;
}

/*!
 * \brief loads Annotation%s for a Project from a given HDF5 file
 * \param file the file that is read from
 * \param proj the Project into which the Annotation%s are read
 * \return true. otherwise an CTFormatException is thrown
 * \throw CTFormatException if iterating over the elements failed
 */
bool ImportHDF5::loadAnnotations(H5File file, std::shared_ptr<Project> proj) {
    Group annotations = file.openGroup("annotations");
    {
        std::shared_ptr<Genealogy> gen = proj->getGenealogy();
        try {
            htri_t ret;

            hsize_t totalSize = getGroupSize(annotations.getId(), "track_annotations") +
                    getGroupSize(annotations.getId(), "object_annotations");
            MessageRelay::emitUpdateDetailMax(static_cast<int>(totalSize));

            ret = H5Lexists(annotations.getId(), "track_annotations", H5P_DEFAULT);
            if (ret >= 0 && ret == true)
                annotations.iterateElems("track_annotations", NULL, process_track_annotations, &(*gen));

            ret = H5Lexists(annotations.getId(), "object_annotations", H5P_DEFAULT);
            if (ret >= 0 && ret == true)
                annotations.iterateElems("object_annotations", NULL, process_object_annotations, &(*gen));

        } catch (H5::GroupIException &e) {
            throw CTFormatException ("Format mismatch while trying to read annotations: " + e.getDetailMsg());
        }
    }
    return true;
}

void ImportHDF5::imageCleanupHandler(void *info) {
    uint8_t *buf = reinterpret_cast<uint8_t*>(info);
    delete[] buf;
}

/*!
 * \brief reads the requested image from a given file
 * \param filename the name of the HDF5 file
 * \param frame the frame, to which the image belongs
 * \param slice the slice, to which the image belongs
 * \param channel the channel, to which the image belongs
 * \return a std::shared_ptr<QImage>, that points to the requested QImage
 */
std::shared_ptr<QImage> ImportHDF5::requestImage (QString filename, int frame, int slice, int channel) {
    H5File file (filename.toStdString().c_str(), H5F_ACC_RDONLY);
    Group imagesGroup = file.openGroup("images");
    Group framesGroup = imagesGroup.openGroup("frames");
    Group frameGroup = framesGroup.openGroup((std::to_string(frame)+"/slices").c_str());
    Group sliceGroup = frameGroup.openGroup((std::to_string(slice)+"/channels").c_str());

    auto data = readMultipleValues<uint8_t>(sliceGroup,std::to_string(channel).c_str());

    uint8_t *buf = std::get<0>(data);
    hsize_t *dims = std::get<1>(data);
    int rank = std::get<2>(data);

    std::shared_ptr<QImage> img;
    int height = static_cast<int>(dims[0]);
    int width = static_cast<int>(dims[1]);
    int depth = rank;

    if (depth == 3) {
        img = std::make_shared<QImage>(buf, width, height, width * sizeof(uint8_t), QImage::Format_RGB888, ImportHDF5::imageCleanupHandler, buf);
    } else {
        img = std::make_shared<QImage>(buf, width, height, width * sizeof(uint8_t), QImage::Format_Grayscale8, imageCleanupHandler, buf);
    }

    delete[] dims;
    /* buffer will be reused by QImage and cleaned up in imageCleanupHandler */
    // delete[] buf;

    return img;
}

/*!
 * \brief Callback for iterating over /images/frames/slices/channels
 * \param group_id callback parameter
 * \param name callback parameter
 * \param op_data callback parameter, holds a pointer to a Slice
 * \return callback status
 */
herr_t ImportHDF5::process_images_frames_slices_channels(hid_t group_id, const char *name, void *op_data) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    Slice *slice = static_cast<Slice *>(op_data);

    if (statbuf.type == H5G_DATASET) {
        std::string sname(name);
        if(std::all_of(sname.begin(),sname.end(),::isdigit)) {
            int channr = atoi(name);
            std::shared_ptr<Channel> channel = slice->getChannel(channr);

            if(channel == nullptr) {
                channel = std::make_shared<Channel>(channr, slice->getSliceId(), slice->getFrameId());
                slice->addChannel(channel);
            }

            DataSet ds = openDataset(group_id, name);
            auto data = readMultipleValues<uint8_t>(ds);
            uint8_t *buf = std::get<0>(data);
            hsize_t *dims = std::get<1>(data);
            int rank = std::get<2>(data);

            std::shared_ptr<QImage> img;
            if (rank == 3) {
                img = bufToImage(buf, dims[0], dims[1], dims[2]);
            } else {
                img = bufToImage(buf, dims[0], dims[1], 1);
            }

            delete[] (buf);
            delete[] (dims);

            channel->setImage(img);
        }
    }
    return 0;
}

/*!
 * \brief Callback for iterating over /images/frames/slices
 * \param group_id callback parameter
 * \param name callback parameter
 * \param op_data callback parameter, holds a pointer to a Frame
 * \return callback status
 */
herr_t ImportHDF5::process_images_frames_slices(hid_t group_id, const char *name, void *op_data) {
    herr_t err = 0;
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    Frame* frame = static_cast<Frame*>(op_data);

    if (statbuf.type == H5G_GROUP) {
        Group group = openGroup(group_id, name);
        int slicenr = readSingleValue<int>(group, "slice_id");

        std::shared_ptr<Slice> slice = frame->getSlice(slicenr);
        if (slice == nullptr) {
            slice = std::make_shared<Slice>(slicenr, frame->getID());
            frame->addSlice(slice);
        }

        err = H5Giterate(group_id,name,NULL,process_images_frames_slices_channels,&(*slice));
    }

    return err;
}

/*!
 * \brief Callback for iterating over /images/frames
 * \param group_id callback parameter
 * \param name callback parameter
 * \param op_data callback parameter, holds a pointer to the Movie
 * \return callback status
 */
herr_t ImportHDF5::process_images_frames(hid_t group_id, const char *name, void *op_data) {
    herr_t err = 0;
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    Movie *movie = static_cast<Movie*>(op_data);

    if (statbuf.type == H5G_GROUP){
        Group frameGroup = openGroup(group_id, name);
        int framenr = readSingleValue<int>(frameGroup, "frame_id");

        /* Check if Frame exists. If it does, use this frame, else create one */
        std::shared_ptr<Frame> frame = movie->getFrame(framenr);
        if (frame == nullptr) {
            frame = std::make_shared<Frame>(framenr);
            movie->addFrame(frame);
        }

        err = H5Giterate(group_id, name, NULL, process_images_frames_slices, &(*frame));
    }
    return err;
}

/*!
 * \brief loads all images for a Project from a given HDF5 file
 * \param file the HDF5 file to read from
 * \param proj the Project to read into
 * \return true, if everything succeeded, false if not
 * \throw CTFormatException if iterating failed
 *
 * \warning This function should not be used. The prefered way is to use CellTracker::ImportHDF5::requestImage, so
 * not all images have to be kept in memory.
 */
bool ImportHDF5::loadImages(H5File file, std::shared_ptr<Project> proj) {
    herr_t err = 0;
    Group images = file.openGroup("images");
    {
        std::shared_ptr<Movie> movie = proj->getMovie();
        try {
            err = H5Giterate(images.getId(),"frames", NULL, process_images_frames,&(*movie));
        } catch (H5::GroupIException &e) {
            throw CTFormatException ("Format mismatch while trying to read images: " + e.getDetailMsg());
        }
    }
    return !err;
}

/*!
 * \brief reads the centroid from a given object Group
 * \param objGroup the object group
 * \return a std::shared_ptr<QPoint> that represents the centroid
 */
std::shared_ptr<QPoint> ImportHDF5::readCentroid(hid_t objGroup) {
    auto point = std::make_shared<QPoint>();

    DataSet ds = openDataset(objGroup, "centroid");
    auto data = readMultipleValues<uint16_t>(ds);
    uint16_t *buf = std::get<0>(data);

    std::shared_ptr<Project::CoordinateSystemInfo> csi = currentProject->getCoordinateSystemInfo();
    switch (csi->getCoordinateSystemType()) {
    case Project::CoordinateSystemInfo::CoordinateSystemType::CST_CARTESIAN: {
        uint32_t iW = csi->getCoordinateSystemData().imageWidth;

        /*! \todo iW produces the right result, but should be iH normally as we are inverting the y-coordinate? */
        point->setX(buf[0]);
        point->setY(iW - buf[1]);
        break; }
    case Project::CoordinateSystemInfo::CoordinateSystemType::CST_QTIMAGE: {
        point->setX(buf[0]);
        point->setY(buf[1]);
        break; }
    }

    delete[] (std::get<1>(data));
    delete[] (buf);

    return point;
}

/*!
 * \brief reads the boundingBox for a given object Group
 * \param objGroup the object Group
 * \return a std::shared_ptr<QRect> that represents the boundingBox
 */
std::shared_ptr<QRect> ImportHDF5::readBoundingBox(hid_t objGroup) {
    auto box = std::make_shared<QRect>();

    DataSet ds = openDataset(objGroup, "bounding_box");
    auto data = readMultipleValues<uint16_t>(ds);
    uint16_t *buf = std::get<0>(data);

    std::shared_ptr<Project::CoordinateSystemInfo> csi = currentProject->getCoordinateSystemInfo();
    switch (csi->getCoordinateSystemType()) {
    case Project::CoordinateSystemInfo::CoordinateSystemType::CST_CARTESIAN: {
        uint32_t iW = csi->getCoordinateSystemData().imageWidth;

        /*! \todo iW produces the right result, but should be iH normally as we are inverting the y-coordinate? */
        box->setCoords(buf[0], iW - buf[1], buf[2], iW - buf[3]);
        break; }
    case Project::CoordinateSystemInfo::CoordinateSystemType::CST_QTIMAGE: {
        box->setCoords(buf[0], buf[1], buf[2], buf[3]);
        break; }
    }

    delete[] (std::get<1>(data));
    delete[] (buf);

    return box;
}

/*!
 * \brief reads the outline for a given object Group
 * \param objGroup the object Group
 * \return a std::shared_ptr<QPolygonF> that represents the outline
 * \warning the QPolygonF is autmatically closed here.
 */
std::shared_ptr<QPolygonF> ImportHDF5::readOutline (hid_t objGroup) {
    auto poly = std::make_shared<QPolygonF>();

    DataSet ds = openDataset(objGroup, "outline");
    auto data = readMultipleValues<uint32_t>(ds);
    uint32_t *buf = std::get<0>(data);
    hsize_t *dims = std::get<1>(data);

    hsize_t length = dims[0];

    for (hsize_t i = 0; i < length; i++) {
        std::shared_ptr<Project::CoordinateSystemInfo> csi = currentProject->getCoordinateSystemInfo();
        switch (csi->getCoordinateSystemType()) {
        case Project::CoordinateSystemInfo::CoordinateSystemType::CST_CARTESIAN: {
            uint32_t iW = csi->getCoordinateSystemData().imageWidth;

            poly->append(QPoint(buf[i*2], iW - buf[i*2 + 1]));
            break; }
        case Project::CoordinateSystemInfo::CoordinateSystemType::CST_QTIMAGE: {
            poly->append(QPoint(buf[i*2], buf[i*2 + 1]));
            break; }
        }
    }
    /* Close the polygon */
    poly->append(poly->first());

    delete[] (buf);
    delete[] (dims);

    return poly;
}

/*!
 * \brief converts a unit8_t[][][] into a QImage
 * \param buf the buffer that holds the image
 * \param height height of the image in pixels
 * \param width width of the image in pixels
 * \param depth depth of the image (1 if grayscale, 3 if rgb)
 * \return a std::shared_ptr<QImage>, that points to the image
 */
std::shared_ptr<QImage> ImportHDF5::bufToImage(uint8_t *buf, hsize_t height, hsize_t width, hsize_t depth)
{
    int offy = static_cast<int>(width*depth);
    int offx = static_cast<int>(depth);

    auto img = std::make_shared<QImage>(width, height, QImage::Format_RGB32);
    QRgb *data = reinterpret_cast<QRgb *>(img->bits());

    for (unsigned int posy=0; posy<height; posy++) {
        for (unsigned int posx=0; posx<width; posx++) {
            unsigned int pxl_idx = posy * offy + posx * offx;
            QColor col;
            if(depth == 3) {
                uint8_t r = buf[pxl_idx + 0];
                uint8_t g = buf[pxl_idx + 1];
                uint8_t b = buf[pxl_idx + 2];
                col.setRgb(r,g,b);
            } else {
                uint8_t c = buf[pxl_idx];
                col.setRgb(c,c,c);
            }
            data[posy * width + posx] = col.rgb();
        }
    }

    return img;
}

/*!
 * \brief Callback for iterating over /objects/frames/\<id\>/slices/\<id\>/channels/\<id\>/objects/\<id\>
 * \param group_id callback parameter
 * \param name callback parameter
 * \param op_data callback parameter, holds a pointer to an Object
 * \return callback status
 */
herr_t ImportHDF5::process_objects_frames_slices_channels_objects (hid_t group_id, const char *name, void *op_data) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    herr_t err = 0;
    std::shared_ptr<Channel> cptr = *static_cast<std::shared_ptr<Channel> *> (op_data);

    if (statbuf.type == H5G_GROUP) {
        Group objGroup = openGroup(group_id, name);
        uint32_t objNr = readSingleValue<uint32_t>(objGroup,"object_id");

        std::shared_ptr<Object> object = cptr->getObject(objNr);

        if (!object) {
            object = std::make_shared<Object>(objNr, cptr);
            cptr->addObject(object);
        }

        std::shared_ptr<QRect> bbox = readBoundingBox(objGroup.getId());
        object->setBoundingBox(bbox);
        std::shared_ptr<QPoint> centroid = readCentroid(objGroup.getId());
        object->setCentroid(centroid);
        std::shared_ptr<QPolygonF> outline = readOutline(objGroup.getId());
        object->setOutline(outline);

        if (groupExists(objGroup, "annotations"))
            annotatedObjects.append(object);
    }

    return err;
}

/*!
 * \brief Callback for iterating over /objects/frames/\<id\>/slices/\<id\>/channels/\<id\>
 * \param group_id callback parameter
 * \param name callback parameter
 * \param op_data callback parameter, holds a pointer to a Frame
 * \return callback status
 */
herr_t ImportHDF5::process_objects_frames_slices_channels (hid_t group_id, const char *name, void *op_data) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    herr_t err = 0;
    Slice *sptr = static_cast<Slice *> (op_data);

    if (statbuf.type == H5G_GROUP) {
        Group channelGrp = openGroup(group_id, name);
        int chanNr = readSingleValue<int>(channelGrp, "channel_id");
        std::shared_ptr<Channel> channel = sptr->getChannel(chanNr);

        if (!channel) {
            channel = std::make_shared<Channel>(chanNr, sptr->getSliceId(), sptr->getFrameId());
            sptr->addChannel(channel);
        }

        Group cGroup = openGroup(group_id, name);
        err = H5Giterate(cGroup.getId(), "objects", NULL, process_objects_frames_slices_channels_objects, &(channel));
    }

    return err;
}

/*!
 * \brief Callback for iterating over /objects/frames/\<id\>/slices/\<id\>
 * \param group_id callback parameter
 * \param name callback parameter
 * \param op_data callback parameter, holds a pointer to a Frame
 * \return callback status
 */
herr_t ImportHDF5::process_objects_frames_slices (hid_t group_id, const char *name, void *op_data) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    herr_t err = 0;
    Frame *fptr = static_cast<Frame *> (op_data);

    if (statbuf.type == H5G_GROUP) {
        Group sliceGrp = openGroup(group_id, name);
        int sliceNr = readSingleValue<int>(sliceGrp, "slice_id");
        std::shared_ptr<Slice>  slice = fptr->getSlice(sliceNr);

        if (!slice) {
            slice = std::make_shared<Slice>(sliceNr, fptr->getID());
            fptr->addSlice(slice);
        }

        std::string sName(name);
        sName.append("/channels");
        err = H5Giterate(group_id, sName.c_str(), NULL, process_objects_frames_slices_channels, &(*slice));
    }

    return err;
}

/*!
 * \brief Callback for iterating over /objects/frames/\<id\>
 * \param group_id callback parameter
 * \param name callback parameter
 * \param op_data callback parameter, holds a pointer to the Movie
 * \return callback status
 */
herr_t ImportHDF5::process_objects_frames(hid_t group_id, const char *name, void *op_data) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    herr_t err = 0;
    Movie *mptr = static_cast<Movie *> (op_data);

    if (statbuf.type == H5G_GROUP){
        Group frameGrp = openGroup(group_id, name);
        int frameNr = readSingleValue<int>(frameGrp, "frame_id");
        std::shared_ptr<Frame> frame = mptr->getFrame(frameNr);

        if (!frame) {
            frame = std::make_shared<Frame>(frameNr);
            mptr->addFrame(frame);
        }

        std::string sName(name);
        sName.append("/slices");
        err = H5Giterate(group_id, sName.c_str(), NULL, process_objects_frames_slices, &(*frame));
    }

    MessageRelay::emitIncreaseDetail();
    return err;
}

/*!
 * \brief loads Object%s for a Project from a given HDF5 file
 * \param file the file that is read from
 * \param proj the Project into which the Object%s are read
 * \return true if everything went fine, false otherwise
 * \throw CTFormatException if iterating over the elements failed
 */
bool ImportHDF5::loadObjects(H5File file, std::shared_ptr<Project> proj) {
    herr_t err = 0;

    Group objects = file.openGroup("objects");
    {
        std::shared_ptr<Movie> movie = proj->getMovie();
        try {
            MessageRelay::emitUpdateDetailMax(static_cast<int>(getGroupSize(objects.getId(),"frames")));
            err = H5Giterate(objects.getId(), "frames", NULL, process_objects_frames, &(*movie));
        } catch (H5::GroupIException &e) {
            throw CTFormatException ("Format mismatch while trying to read objects: " + e.getDetailMsg());
        }
    }
    return !err;
}

/*!
 * \brief Callback for iterating over /autotracklets/\<id\>/objects/\<id\>
 * \param group_id callback parameter
 * \param name callback parameter
 * \param opdata callback parameter, holds a pointer to the Project
 * \return callback status
 * \throw CTMissingElementException if one or more elements of this Tracklet could not be found
 */
herr_t ImportHDF5::process_autotracklets_objects(hid_t group_id, const char *name, void *opdata) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    std::pair<std::shared_ptr<AutoTracklet>,Project*>* p = static_cast<std::pair<std::shared_ptr<AutoTracklet>,Project*>*>(opdata);
    std::shared_ptr<AutoTracklet> autotracklet = p->first;
    Project *project = p->second;

    if (statbuf.type == H5G_GROUP) {
        Group objGroup = openGroup(group_id, name);

        uint32_t oId = readSingleValue<uint32_t>(objGroup, "object_id");
        uint32_t fId = readSingleValue<uint32_t>(objGroup, "frame_id");
        uint32_t cId = readSingleValue<uint32_t>(objGroup, "channel_id");
        uint32_t sId = readSingleValue<uint32_t>(objGroup, "slice_id");

        std::shared_ptr<Frame> frame = project->getMovie()->getFrame(fId);
        if (frame == nullptr)
            throw CTMissingElementException("Did not find frame " + std::to_string(fId) + " in Movie");

        std::shared_ptr<Slice> slice = frame->getSlice(sId);
        if (slice == nullptr)
            throw CTMissingElementException("Did not find slice " + std::to_string(sId) + " in Frame");

        std::shared_ptr<Object> object = slice->getChannel(cId)->getObject(oId);
        if (object == nullptr)
            throw CTMissingElementException("Did not find object " + std::to_string(oId) + " in slice " + std::to_string(sId) + " of frame " + std::to_string(fId));

        if (object != nullptr && frame != nullptr) {
            autotracklet->addComponent(frame, object);
        } else {
            throw CTMissingElementException("Error while adding object " + std::to_string(oId)
                    + " at frame " + std::to_string(fId)
                    + "to track" + std::to_string(autotracklet->getID()));
        }
    }

    return 0;
}

herr_t ImportHDF5::process_autotracklets_events_ids(hid_t group_id, const char *name, void *opdata) {
    std::list<int> *names = static_cast<std::list<int>*>(opdata);

    Group daughter = openGroup(group_id, name);
    int dId = readSingleValue<int>(daughter, "autotracklet_id");
    names->push_back(dId);

    return 0;
}

/*!
 * \brief Callback for iterating over the TrackEventDivision-events of /tracklets
 * \param group_id_o callback parameter
 * \param name callback parameter
 * \param opdata callback parameter, holds a pointer to the Project
 * \return callback status
 * \throw CTMissingElementException if the tracklet could not be found in the Genealogy
 */
herr_t ImportHDF5::process_autotracklets_events(hid_t group_id_o, const char *name, void *opdata) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id_o, name, true, &statbuf);
    Project *project = static_cast<Project*> (opdata);
    Group group = openGroup(group_id_o, name);

    if (statbuf.type == H5G_GROUP) {
        herr_t err;
        uint32_t atId = readSingleValue<uint32_t>(group, "autotracklet_id");
        std::shared_ptr<AutoTracklet> at = project->getAutoTracklet(atId);

        if (at && linkExists(group, "next_event") && linkExists(group, "next")) {
            /* get event type */
            Group nextEv = group.openGroup("next_event");

            std::string evName = readString(nextEv, "name");

            if (evName.compare("cell_division") == 0) {
                auto ted = std::make_shared<TrackEventDivision<AutoTracklet>>();
                ted->setPrev(at);
                std::list<int> nextIds;
                err = H5Giterate(group.getId(), "next", NULL, process_autotracklets_events_ids, &nextIds);
                if (err != 0)
                    return err;
                auto nList = std::make_shared<QList<std::weak_ptr<AutoTracklet>>>();

                for (int id: nextIds) {
                    std::shared_ptr<AutoTracklet> d = project->getAutoTracklet(id);
                    if (d) {
                        d->setPrev(ted);
                        nList->append(d);
                    } else {
                        qDebug() << "Did not find Daughter-Tracklet"
                                 << id << "in genealogy, which is required by autotracklet"
                                 << at->getID() << "(group" << name << ")";

                    }
                }
                ted->setNext(nList);
                at->setNext(ted);
            } else {
                qDebug() << "unhandled event in autotracklet" << name << ":" << evName.c_str();
            }
        }
    }

    MessageRelay::emitIncreaseDetail();
    return 0;
}

/*!
 * \brief Callback for iterating over /autotracklets/\<id\>
 * \param group_id callback parameter
 * \param name callback parameter
 * \param op_data callback parameter, holds a pointer to the Project
 * \return callback status
 */
herr_t ImportHDF5::process_autotracklets (hid_t group_id, const char *name, void *op_data) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    herr_t err = 0;
    Project *project = static_cast<Project*>(op_data);

    if (statbuf.type == H5G_GROUP) {
        Group trackGroup = openGroup(group_id, name);
        int atnr = readSingleValue<int>(trackGroup, "autotracklet_id");

        std::shared_ptr<AutoTracklet> autoTracklet;

        autoTracklet = std::make_shared<AutoTracklet>(atnr);
        project->addAutoTracklet(autoTracklet);

        std::pair<std::shared_ptr<AutoTracklet>,Project*> p(autoTracklet,project);
        /* add the objects to this autotracklet */
        err = H5Giterate(trackGroup.getId(), "objects", NULL, process_autotracklets_objects, &(p));
    }

    MessageRelay::emitIncreaseDetail();
    return err;
}

herr_t ImportHDF5::process_tracklets_events_ids(hid_t group_id, const char *name, void *opdata) {
    std::list<int> *names = static_cast<std::list<int>*>(opdata);

    Group daughter = openGroup(group_id, name);
    int dId = readSingleValue<int>(daughter, "tracklet_id");
    names->push_back(dId);

    return 0;
}

/*!
 * \brief Callback for iterating over events in /tracklets/\<id\>/
 * \param group_id_o callback parameter
 * \param name callback parameter
 * \param opdata callback parameter, holds a pointer to the Project
 * \return callback status
 */
herr_t ImportHDF5::process_tracklets_events(hid_t group_id_o, const char *name, void *opdata) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id_o, name, true, &statbuf);
    Project *project = static_cast<Project*> (opdata);
    Group group = openGroup(group_id_o, name);

    if (statbuf.type == H5G_GROUP) {
        herr_t err;
        uint32_t tId = readSingleValue<uint32_t>(group, "tracklet_id");
        std::shared_ptr<Tracklet> tracklet = project->getGenealogy()->getTracklet(tId);

        if (tracklet && linkExists(group, "next_event")) {
            bool nextGroupExists = groupExists(group, "next");
            /* get event type */
            Group nextEv = group.openGroup("next_event");

            std::string evName = readString(nextEv, "name");

            if (evName.compare("cell_death") == 0) {
                auto ted = std::make_shared<TrackEventDead<Tracklet>>();
                ted->setPrev(tracklet);
                tracklet->setNext(ted);
            } else if (evName.compare("cell_division") == 0) {
                auto ted = std::make_shared<TrackEventDivision<Tracklet>>();
                ted->setPrev(tracklet);
                std::list<int> nextIds;
                if (nextGroupExists) {
                    err = H5Giterate(group.getId(), "next", NULL, process_tracklets_events_ids, &nextIds);
                    if (err != 0)
                        return err;
                }
                auto nList = std::make_shared<QList<std::weak_ptr<Tracklet>>>();

                for (int id: nextIds) {
                    std::shared_ptr<Tracklet> d = project->getGenealogy()->getTracklet(id);
                    if (d) {
                        d->setPrev(ted);
                        nList->append(d);
                    } else {
                        qDebug() << "Did not find Daughter-Tracklet"
                                 << id << "in genealogy, which is required by tracklet"
                                 << tracklet->getId() << "(group" << name << ")";
                    }
                }
                ted->setNext(nList);
                tracklet->setNext(ted);
            } else if (evName.compare("cell_lost") == 0) {
                auto tel = std::make_shared<TrackEventLost<Tracklet>>();
                tel->setPrev(tracklet);
                tracklet->setNext(tel);
            } else if (evName.compare("cell_merge") == 0) {
                Group nextGrp = group.openGroup("next");
                std::list<std::string> names = collectGroupElementNames(nextGrp);
                if (names.size() != 1) /* there should only be one next tracklet */
                    throw CTImportException("the next group of tracklet " + std::to_string(tId) + " contained zero or more than one tracklets");
                Group next = nextGrp.openGroup(names.front());
                if (!groupExists(next, "previous_event"))
                    throw CTImportException("the tracklet following tracklet " + std::to_string(tId) + " does not contain a previous_event");

                std::string nEvName = readString(next, "previous_event/name");

                if (nEvName.compare("cell_merge") != 0)
                    throw CTImportException("the event in the tracklet following " + std::to_string(tId) + " does not have \'cell_merge\' as previous_event");

                uint32_t nId = readSingleValue<uint32_t>(next, "tracklet_id");
                std::shared_ptr<Tracklet> n = project->getGenealogy()->getTracklet(nId);
                if (!n)
                    throw CTImportException("the tracklet following tracklet " + std::to_string(tId) + "could not be found in the genealogy");
                std::shared_ptr<TrackEventMerge<Tracklet>> tem;
                /* check if event exists already and if not, create it */
                if (!n->getPrev()) {
                    tem = std::make_shared<TrackEventMerge<Tracklet>>();
                    tem->setNext(n);
                    n->setPrev(tem);
                } else if (n->getPrev()->getType() == TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_MERGE) {
                    tem = std::static_pointer_cast<TrackEventMerge<Tracklet>>(n->getPrev());
                } else {
                    throw CTImportException("the next tracklet to tracklet " + std::to_string(tId) + "does already have a previous event and it is not of type TrackEventMerge");
                }
                bool contained = false;
                for (std::weak_ptr<Tracklet> t : *tem->getPrev()) {
                    if (t.lock() == tracklet) {
                        contained = true;
                        break;
                    }
                }
                if (!contained)
                    tem->getPrev()->append(tracklet);
                tracklet->setNext(tem);
            } else if (evName.compare("cell_unmerge") == 0) {
                auto teu = std::make_shared<TrackEventUnmerge<Tracklet>>();
                teu->setPrev(tracklet);
                std::list<int> nextIds;
                if (nextGroupExists) {
                    err = H5Giterate(group.getId(), "next", NULL, process_tracklets_events_ids, &nextIds);
                    if (err != 0)
                        return err;
                }
                auto nList = std::make_shared<QList<std::weak_ptr<Tracklet>>>();

                for (int id: nextIds) {
                    std::shared_ptr<Tracklet> d = project->getGenealogy()->getTracklet(id);
                    if (d) {
                        d->setPrev(teu);
                        nList->append(d);
                    } else {
                        qDebug() << "Did not find Unmerged-Tracklet"
                                 << id << "in genealogy, which is required by tracklet"
                                 << tracklet->getId() << "(group" << name << ")";
                    }
                }
                teu->setNext(nList);
                tracklet->setNext(teu);
            } else if (evName.compare("end_of_movie") == 0) {
                auto teeom = std::make_shared<TrackEventEndOfMovie<Tracklet>>();
                teeom->setPrev(tracklet);
                tracklet->setNext(teeom);
            } else {
                qDebug() << "unhandled event in autotracklet" << name << ":" << evName.c_str();
            }
        }
    }

    MessageRelay::emitIncreaseDetail();
    return 0;
}

/*!
 * \brief Callback for iterating over events in /tracklets/\<id\>/objects/\<id\>
 * \param group_id callback parameter
 * \param name callback parameter
 * \param opdata callback parameter, holds a pointer to the Project
 * \return callback status
 */
herr_t ImportHDF5::process_tracklets_objects(hid_t group_id, const char *name, void *opdata) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    std::pair<std::shared_ptr<Tracklet>,Project*>* p = static_cast<std::pair<std::shared_ptr<Tracklet>,Project*>*>(opdata);
    std::shared_ptr<Tracklet> tracklet = p->first;
    Project *project = p->second;

    if (statbuf.type == H5G_GROUP) {
        if (!linkExists(group_id, name) || !linkValid(group_id, name))
            return 0; /* continue */

        Group objGroup = openGroup(group_id, name);

        uint32_t oId = readSingleValue<uint32_t>(objGroup, "object_id");
        uint32_t fId = readSingleValue<uint32_t>(objGroup, "frame_id");
        uint32_t cId = readSingleValue<uint32_t>(objGroup, "channel_id");
        uint32_t sId = readSingleValue<uint32_t>(objGroup, "slice_id");

        std::shared_ptr<Frame> frame = project->getMovie()->getFrame(fId);
        std::shared_ptr<Object> object = frame->getSlice(sId)->getChannel(cId)->getObject(oId);

        if (frame == nullptr)
            throw CTMissingElementException("Did not find frame " + std::to_string(fId) + " in Movie");
        if (object == nullptr)
            throw CTMissingElementException("Did not find object " + std::to_string(oId) + " in slice " + std::to_string(sId) + " of frame " + std::to_string(fId));

        if (object != nullptr && frame != nullptr) {
            tracklet->addToContained(frame,object);
        } else {
            throw CTMissingElementException("Error while adding object " + std::to_string(oId)
                    + " at frame " + std::to_string(fId)
                    + "to track" + std::to_string(tracklet->getId()));
        }
    }

    return 0;
}

/*!
 * \brief Callback for iterating over events in /tracklets/\<id\>
 * \param group_id callback parameter
 * \param name callback parameter
 * \param op_data callback parameter, holds a pointer to the Project
 * \return callback status
 */
herr_t ImportHDF5::process_tracklets (hid_t group_id, const char *name, void *op_data) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    herr_t err = 0;
    Project *project = static_cast<Project*>(op_data);

    if (statbuf.type == H5G_GROUP) {
        Group trackGroup = openGroup(group_id, name);
        int atnr = readSingleValue<int>(trackGroup, "tracklet_id");

        std::shared_ptr<Tracklet> tracklet = project->getGenealogy()->getTracklet(atnr);

        if (!tracklet) {
            tracklet = std::make_shared<Tracklet>();
            tracklet->setId(atnr);
            project->getGenealogy()->addTracklet(tracklet);
        }

        if (groupExists(trackGroup, "annotations"))
            annotatedTracklets.append(tracklet);

        std::pair<std::shared_ptr<Tracklet>,Project*> p(tracklet,project);
        /* add the objects to this tracklet */
        err = H5Giterate(trackGroup.getId(), "objects", NULL, process_tracklets_objects, &(p));
    }

    MessageRelay::emitIncreaseDetail();
    return err;
}

/*!
 * \brief loads AutoTracklet%s for a Project from a given HDF5 file
 * \param file the file that is read from
 * \param proj the Project into which the AutoTracklet%s are read
 * \return true, if everything went fine, false otherwise
 * \throw CTFormatException if iterating over the elements failed
 */
bool ImportHDF5::loadAutoTracklets(H5File file, std::shared_ptr<Project> proj) {
    herr_t err = 0;

    try {
        MessageRelay::emitUpdateDetailMax(static_cast<int>(getGroupSize(file.getId(),"autotracklets")));
        err = H5Giterate(file.getId(), "autotracklets", NULL, process_autotracklets, &(*proj));
    } catch (H5::GroupIException &e) {
        throw CTFormatException ("Format mismatch while trying to read autotracklets: " + e.getDetailMsg());
    }

    return !err;
}

/*!
 * \brief loads Tracklet%s for a Project from a given HDF5 file
 * \param file the file that is read from
 * \param proj the Project into which the Tracklet%s are read
 * \return true, if everything went fine, false otherwise
 * \throw CTFormatException if iterating over the elements failed
 */
bool ImportHDF5::loadTracklets(H5File file, std::shared_ptr<Project> proj)
{
    herr_t err = 0;

    try {
        if (groupExists(file, "tracklets")) {
            MessageRelay::emitUpdateDetailMax(static_cast<int>(getGroupSize(file.getId(),"tracklets")));
            err = H5Giterate(file.getId(), "tracklets", NULL, process_tracklets, &(*proj));
        }
    } catch (H5::GroupIException &e) {
        throw CTFormatException ("Format mismatch while trying to read tracklets: " + e.getDetailMsg());
    }

    return !err;
}

/*!
 * \brief loads TrackEvent%s
 * \param file the file that is read from
 * \param proj the Project into which the Annotation%s-assignments are read
 * \return true, if everything went fine, false otherwise
 * \throw CTFormatException if iterating over the elements failed
 */
bool ImportHDF5::loadEventInstances(H5File file, std::shared_ptr<Project> proj) {
    herr_t err1 = 0, err2 = 0;

    try {
        int total = static_cast<int>(getGroupSize(file.getId(), "autotracklets"));
        if (groupExists(file, "tracklets"))
            total += getGroupSize(file.getId(), "tracklets");
        MessageRelay::emitUpdateDetailMax(total);
        err1 = H5Giterate(file.getId(), "autotracklets", NULL, process_autotracklets_events, &(*proj));
        if (groupExists(file, "tracklets"))
            err2 = H5Giterate(file.getId(), "tracklets", NULL, process_tracklets_events, &(*proj));
    } catch (H5::GroupIException &e) {
        throw CTFormatException ("Format mismatch while trying to read mother-daughter relations: " + e.getDetailMsg());
    }

    return !err1 && !err2;
}

bool ImportHDF5::loadAnnotationAssignments(H5File file, std::shared_ptr<Project> proj)
{
    MessageRelay::emitUpdateDetailMax(annotatedObjects.length() + annotatedTracklets.length());
    for (std::shared_ptr<Object> o : annotatedObjects) {
        uint32_t fId = o->getFrameId();
        uint32_t sId = o->getSliceId();
        uint32_t cId = o->getChannelId();
        uint32_t oId = o->getId();

        std::string objectPath = "/objects/frames/"+std::to_string(fId)
                +"/slices/"+std::to_string(sId)
                +"/channels/"+std::to_string(cId)
                +"/objects/"+std::to_string(oId);
        Group annotationsGroup = file.openGroup(objectPath + "/annotations");
        std::list<std::string> names = collectGroupElementNames(annotationsGroup);
        for (std::string name : names) {
            Group annotationGroup = annotationsGroup.openGroup(name);
            uint32_t id = readSingleValue<uint32_t>(annotationGroup, "object_annotation_id");
            std::shared_ptr<Annotation> annotation = proj->getGenealogy()->getAnnotation(id);
            proj->getGenealogy()->annotate(o, annotation);
        }

        MessageRelay::emitIncreaseDetail();
    }
    for (std::shared_ptr<Tracklet> t : annotatedTracklets) {
        uint32_t tId = t->getId();

        std::string trackletPath = "/tracklets/" + std::to_string(tId);
        Group annotationsGroup = file.openGroup(trackletPath + "/annotations");
        std::list<std::string> names = collectGroupElementNames(annotationsGroup);
        for (std::string name : names) {
            Group annotationGroup = annotationsGroup.openGroup(name);
            uint32_t id = readSingleValue<uint32_t>(annotationGroup, "track_annotation_id");
            std::shared_ptr<Annotation> annotation = proj->getGenealogy()->getAnnotation(id);
            proj->getGenealogy()->annotate(t, annotation);
        }
        MessageRelay::emitIncreaseDetail();
    }
    return true;
}


/*!
 * \brief tests if the groupname in a HDF5 file matches the value of the contained dataset object_id
 * \param file the file to check
 * \param checkee runtime object that holds information about the current object
 * \param prefix the current prefix
 * \param err error buffer to write back to if the test failed
 * \return true if the test was passed, false otherwise
 */
bool Validator::test_groupname_matches_object_id(H5::H5File file, checkObject checkee, std::string prefix, std::string &err) {
    std::string groupName = checkee.name;
    Group objectGrp = file.openGroup(prefix + "/" + groupName);
    int objectId = readSingleValue<int>(objectGrp, "object_id");
    if (std::atoi(groupName.c_str()) == objectId)
        return true;
    err = "object_id " + std::to_string(objectId) + " mismatches groupname "+groupName;
    return false;
}

/*!
 * \brief tests if the groupname in a HDF5 file matches the value of the contained dataset channel_id
 * \param file the file to check
 * \param checkee runtime object that holds information about the current object
 * \param prefix the current prefix
 * \param err error buffer to write back to if the test failed
 * \return true if the test was passed, false otherwise
 */
bool Validator::test_groupname_matches_channel_id(H5::H5File file, checkObject checkee, std::string prefix, std::string &err) {
    std::string groupName = checkee.name;
    Group objectGrp = file.openGroup(prefix + "/" + groupName);
    int objectId = readSingleValue<int>(objectGrp, "channel_id");
    if (std::atoi(groupName.c_str()) == objectId)
        return true;
    err = "channel_id " + std::to_string(objectId) + " mismatches groupname "+groupName;
    return false;
}

/*!
 * \brief tests if the groupname in a HDF5 file matches the value of the contained dataset slice_id
 * \param file the file to check
 * \param checkee runtime object that holds information about the current object
 * \param prefix the current prefix
 * \param err error buffer to write back to if the test failed
 * \return true if the test was passed, false otherwise
 */
bool Validator::test_groupname_matches_slice_id(H5::H5File file, checkObject checkee, std::string prefix, std::string &err) {
    std::string groupName = checkee.name;
    Group objectGrp = file.openGroup(prefix + "/" + groupName);
    int objectId = readSingleValue<int>(objectGrp, "slice_id");
    if (std::atoi(groupName.c_str()) == objectId)
        return true;
    err = "slice_id " + std::to_string(objectId) + " mismatches groupname "+groupName;
    return false;
}

/*!
 * \brief tests if the groupname in a HDF5 file matches the value of the contained dataset frame_id
 * \param file the file to check
 * \param checkee runtime object that holds information about the current object
 * \param prefix the current prefix
 * \param err error buffer to write back to if the test failed
 * \return true if the test was passed, false otherwise
 */
bool Validator::test_groupname_matches_frame_id(H5::H5File file, checkObject checkee, std::string prefix, std::string &err) {
    std::string groupName = checkee.name;
    Group objectGrp = file.openGroup(prefix + "/" + groupName);
    int objectId = readSingleValue<int>(objectGrp, "frame_id");
    if (std::atoi(groupName.c_str()) == objectId)
        return true;
    err = "frame_id " + std::to_string(objectId) + " mismatches groupname "+groupName;
    return false;
}

/*!
 * \brief tests if the groupname in a HDF5 file matches the value of the contained dataset tracklet_id
 * \param file the file to check
 * \param checkee runtime object that holds information about the current object
 * \param prefix the current prefix
 * \param err error buffer to write back to if the test failed
 * \return true if the test was passed, false otherwise
 */
bool Validator::test_groupname_matches_tracklet_id(H5::H5File file, checkObject checkee, std::string prefix, std::string &err) {
    std::string groupName = checkee.name;
    Group objectGrp = file.openGroup(prefix + "/" + groupName);
    int objectId = readSingleValue<int>(objectGrp, "tracklet_id");
    if (std::atoi(groupName.c_str()) == objectId)
        return true;
    err = "tracklet_id " + std::to_string(objectId) + " mismatches groupname "+groupName;
    return false;
}

/*!
 * \brief tests if the groupname in a HDF5 file matches the value of the contained dataset autotracklet_id
 * \param file the file to check
 * \param checkee runtime object that holds information about the current object
 * \param prefix the current prefix
 * \param err error buffer to write back to if the test failed
 * \return true if the test was passed, false otherwise
 */
bool Validator::test_groupname_matches_autotracklet_id(H5::H5File file, checkObject checkee, std::string prefix, std::string &err) {
    std::string groupName = checkee.name;
    Group objectGrp = file.openGroup(prefix + "/" + groupName);
    int objectId = readSingleValue<int>(objectGrp, "autotracklet_id");
    if (std::atoi(groupName.c_str()) == objectId)
        return true;
    err = "autotracklet_id " + std::to_string(objectId) + " mismatches groupname "+groupName;
    return false;
}

/*!
 * \brief Checks if a given file is a valid CellTracker project file and coheres to certain basic constraints.
 * \param fileName the filename of the file to check
 * \param warnType warn, if some part is not of the expected type (group, dataset)
 * \param warnLink warn, if some part is not of the expected link type (soft, hard)
 * \param warnTest warn, if some part doesn't pass the test function, that is associated with it
 * \return true, if this is a valid CellTracker-File, false otherwise
 */
bool Validator::validCellTrackerFile(QString fileName, bool warnType, bool warnLink, bool warnTest)
{
    bool valid = true;
    { /* Check if valid HDF5 file */
        if (!H5File::isHdf5(fileName.toStdString())) {
            qDebug() << "invalid file" << fileName;
            return false;
        }
    }
    H5File file(fileName.toStdString().c_str(), H5F_ACC_RDONLY);
    { /* Check if all required Groups/DataSets exist */
        checkObject cProj = { "", true, H5L_TYPE_HARD, TYPE_GROUP, nullptr,
                              {{"annotations", false, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                {"object_annotations", false, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                 {"*", false, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                  {"description", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                  {"object_annotation_id", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                  {"title", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}}}}}},
                                {"track_annotations", false, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                 {"*", false, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                  {"description", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                  {"track_annotation_id", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                  {"title", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}}}}}}}},
                               {"autotracklets", true, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                {"*", false, H5L_TYPE_HARD, TYPE_GROUP, test_groupname_matches_autotracklet_id, {
                                 {"autotracklet_id", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                 {"start", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                 {"end", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                 {"next_event", false, H5L_TYPE_SOFT, TYPE_GROUP, nullptr, {}},
                                 {"previous_event", false, H5L_TYPE_SOFT, TYPE_GROUP, nullptr, {}},
                                 {"objects", true, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                  {"*", false, H5L_TYPE_SOFT, TYPE_GROUP, nullptr, {}}}},
                                 {"next", false, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                  {"*", false, H5L_TYPE_SOFT, TYPE_GROUP, nullptr, {}}}},
                                 {"previous", false, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                  {"*", false, H5L_TYPE_SOFT, TYPE_GROUP, nullptr, {}}}}}}}},
                               {"events", true, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                {"cell_death", true, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                 {"description", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                 {"event_id", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                 {"name", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}}}},
                                {"cell_division", true, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                 {"description", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                 {"event_id", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                 {"name", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}}}},
                                {"cell_lost", true, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                 {"description", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                 {"event_id", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                 {"name", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}}}},
                                {"cell_merge", true, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                 {"description", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                 {"event_id", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                 {"name", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}}}},
                                {"cell_unmerge", true, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                 {"description", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                 {"event_id", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                 {"name", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}}}},
                                {"end_of_movie", true, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                 {"description", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                 {"event_id", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                 {"name", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}}}}}},
                               {"images", true, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                {"frames", true, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                 {"*", false, H5L_TYPE_HARD, TYPE_GROUP, test_groupname_matches_frame_id, {
                                  {"slices", true, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                   {"*", false, H5L_TYPE_HARD, TYPE_GROUP, test_groupname_matches_slice_id, {
                                    {"channels", true, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                     {"*", false, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}}}},
                                    {"dimensions", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                    {"nchannels", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                    {"slice_id", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}}}}}},
                                  {"frame_id", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}}}}}},
                                {"frame_rate", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                {"nframes", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                {"nslices", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                {"slice_shape", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}}}},
                               {"info", false, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {}},
                               {"objects", true, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                {"frames", true, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                 {"*", false, H5L_TYPE_HARD, TYPE_GROUP, test_groupname_matches_frame_id, {
                                  {"slices", true, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                   {"*", false, H5L_TYPE_HARD, TYPE_GROUP, test_groupname_matches_slice_id, {
                                    {"channels", true, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                     {"*", false, H5L_TYPE_HARD, TYPE_GROUP, test_groupname_matches_channel_id, {
                                      {"objects", true, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                       {"*", false, H5L_TYPE_HARD, TYPE_GROUP, test_groupname_matches_object_id, {
                                        {"annotations", false, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                         {"*", false, H5L_TYPE_SOFT, TYPE_GROUP, nullptr, {}}}},
                                        {"bounding_box", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                        {"centroid", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                        {"channel_id", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                        {"frame_id", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                        {"object_id", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                        {"outline", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                        {"packed_mask", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                        {"slice_id", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}}}}}},
                                      {"channel_id", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}}}}}},
                                    {"dimensions", true, H5L_TYPE_SOFT, TYPE_DATASET, nullptr, {}},
                                    {"nchannels", true, H5L_TYPE_SOFT, TYPE_DATASET, nullptr, {}},
                                    {"slice_id", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}}}}}},
                                  {"frame_id", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}}}}}},
                                {"frame_rate", true, H5L_TYPE_SOFT, TYPE_DATASET, nullptr, {}},
                                {"nframes", true, H5L_TYPE_SOFT, TYPE_DATASET, nullptr, {}},
                                {"nslices", true, H5L_TYPE_SOFT, TYPE_DATASET, nullptr, {}},
                                {"slice_shape", true, H5L_TYPE_SOFT, TYPE_DATASET, nullptr, {}}}},
                               {"tracklets", false, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                {"*", false, H5L_TYPE_HARD, TYPE_GROUP, test_groupname_matches_tracklet_id, {
                                 {"tracklet_id", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                 {"start", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                 {"end", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
                                 {"next_event", false, H5L_TYPE_SOFT, TYPE_GROUP, nullptr, {}},
                                 {"previous_event", false, H5L_TYPE_SOFT, TYPE_GROUP, nullptr, {}},
                                 {"objects", true, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                  {"*", false, H5L_TYPE_SOFT, TYPE_GROUP, nullptr, {}}}},
                                 {"annotations", false, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                  {"*", false, H5L_TYPE_SOFT, TYPE_GROUP, nullptr, {}}}},
                                 {"next", false, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                  {"*", false, H5L_TYPE_SOFT, TYPE_GROUP, nullptr, {}}}},
                                 {"previous", false, H5L_TYPE_HARD, TYPE_GROUP, nullptr, {
                                  {"*", false, H5L_TYPE_SOFT, TYPE_GROUP, nullptr, {}}}}}}}}}};

        std::list<workItem> workQueue = {{"", cProj}};
        while (!workQueue.empty()) {
            workItem currentWork = workQueue.front();
            workQueue.pop_front();
            std::string currentPrefix = currentWork.prefix;
            checkObject currentObject = currentWork.item;
            std::string currentFullName = currentPrefix + "/" + currentObject.name;
            std::list<workItem> newWork;
            bool checkExistence = true;

            /* Check current item */
            if (currentObject.name.compare("") == 0 && currentPrefix.compare("") == 0) {
                /* root */
                currentFullName = "";
                checkExistence = false;
            } else if (currentObject.name.compare("*") == 0) {
                /* wildcard */
                checkExistence = false;
            }

            if (checkExistence) {
                /* check existence */
                if (!linkExists(file, currentFullName.c_str())) {
                    if (currentObject.necessary)
                        qDebug() << currentFullName.c_str() << "does not exist though it is necessary";
                    /* continue with the next item in the queue, as this item does not exist, it also doesn't have children */
                    continue;
                }

                /* check type */
                if (warnType && currentObject.type == TYPE_GROUP) {
                    if (!groupExists(file, currentFullName.c_str()))
                        qDebug() << currentFullName.c_str() << "is not a group";
                    /* check link */
                    Group g = file.openGroup(currentFullName);
                    if (warnLink && currentObject.link == H5L_TYPE_SOFT && getLinkType(g) != H5L_TYPE_SOFT) {
                        qDebug() << currentFullName.c_str() << "is not a soft link";
                        valid = false;
                    } else if (warnLink && currentObject.link == H5L_TYPE_HARD && getLinkType(g) != H5L_TYPE_HARD) {
                        qDebug() << currentFullName.c_str() << "is not a hard link";
                        valid = false;
                    }
                    /* test function */
                    std::string errBuf;
                    if (warnTest && currentObject.test && !currentObject.test(file, currentObject, currentPrefix, errBuf))
                        qDebug() << currentFullName.c_str() << "did not pass the test function with the following error message:" << errBuf.c_str();
                } else if (warnType && currentObject.type == TYPE_DATASET) {
                    if (!datasetExists(file, currentFullName.c_str()))
                        qDebug() << currentFullName.c_str() << "is not a dataset";
                    /* check link */
                    DataSet d = file.openDataSet(currentFullName);
                    if (warnLink && currentObject.link == H5L_TYPE_SOFT && getLinkType(d) != H5L_TYPE_SOFT) {
                        qDebug() << currentFullName.c_str() << "is not a soft link";
                        valid = false;
                    } else if (warnLink && currentObject.link == H5L_TYPE_HARD && getLinkType(d) != H5L_TYPE_HARD) {
                        qDebug() << currentFullName.c_str() << "is not a hard link";
                        valid = false;
                    }
                    /* test function */
                    std::string errBuf;
                    if (warnTest && currentObject.test && !currentObject.test(file, currentObject, currentPrefix, errBuf))
                        qDebug() << currentFullName.c_str() << "did not pass the test function with the following error message:" << errBuf.c_str();
                }
            }

            /* collect children */
            if (currentObject.name.compare("*") == 0) {
                /* wildcard */
                Group grp = file.openGroup(currentPrefix);
                std::list<std::string> childrenNames = collectGroupElementNames(grp);
                for (std::string childName : childrenNames) {
                    checkObject wildcardObject = {childName,
                                                  currentObject.necessary,
                                                  currentObject.link,
                                                  currentObject.type,
                                                  currentObject.test,
                                                  {}};
                    workItem self = { currentPrefix, wildcardObject};
                    newWork.push_front(self);
                    for (checkObject c : currentObject.dependents) {
                        workItem nI = {currentPrefix + "/" + childName, c};
                        newWork.push_front(nI);
                    }
                }
            } else {
                /* regular */
                for (checkObject c : currentObject.dependents){
                    workItem nI = { currentFullName, c };
                    newWork.push_back(nI);
                }
            }

            /* enqueue children */
            if (!newWork.empty())
                std::copy(newWork.begin(), newWork.end(), std::front_insert_iterator<std::list<workItem>>(workQueue));
        }
    }
    return valid;
}



}
