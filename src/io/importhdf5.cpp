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
#include "tracked/trackeventdivision.h"
#include "exceptions/ctimportexception.h"
#include "exceptions/ctformatexception.h"
#include "exceptions/ctmissingelementexception.h"
#include "provider/messagerelay.h"

namespace CellTracker {
using namespace H5;

std::shared_ptr<Project> currentProject;

ImportHDF5::ImportHDF5() {}

ImportHDF5::~ImportHDF5() {}

/*!
 * \brief loads a Project from a HDF5 file
 * \param fileName the filename of the HDF5 file
 * \return a std::shared_ptr<Project> to the Project
 * \throw CTImportException if any of the phases fails
 *
 * Loading a project is done in different phases:
 *   - CellTracker::ImportHDF5::loadObjects
 *   - CellTracker::ImportHDF5::loadTracklets (w/o mother-daugther relation)
 *   - CellTracker::ImportHDF5::loadDaughterRelations
 *   - CellTracker::ImportHDF5::loadAnnotations
 *
 * Images are loaded seperately by invoking CellTracker::ImportHDF5::requestImage.
 */
std::shared_ptr<Project> ImportHDF5::load(QString fileName)
{
    std::shared_ptr<Project> proj;

    try {
        H5File file(fileName.toStdString().c_str(),H5F_ACC_RDONLY);

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
                {loadInfo,           "project information"},
                {loadEvents,         "events"},
                {loadObjects,        "objects"},
                {loadAutoTracklets,  "autotracklets"},
                {loadEventInstances, "mother-daughter relations"},
                {loadTracklets,      "tracklets"},
                {loadAnnotations,    "annotations"}
        };

        MessageRelay::emitUpdateOverallName("Importing from HDF5");
        MessageRelay::emitUpdateOverallMax(phases.size());

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
    } catch (H5::FileIException &e) {
        throw CTImportException ("Opening the file " + fileName.toStdString() + " failed: " + e.getDetailMsg());
    }

    return proj;
}


/*!
 * \brief loads information about the project from a given HDF5 file
 * \param file the file, that is read from
 * \param proj the std::shared_ptr<Project> into which the information is loaded
 * \return true, if successfull, false otherwise
 * \throw CTFormatException if one of the neccessary datasets or groups in "info" does not exist
 *
 * This function opens the group "/info" in the file and reads various information about the project.
 * The general structure is assumed to be
 * \verbatim
 /info
      timeOfConversion
      inputFiles (doesn't work yet)
      tracking_info
              algorithm
              ilastik_version
              timeOfTracking
 \endverbatim
 * If one or more of those groups and datasets don't exist, a CTMissingElementException will be thrown.
 */
bool ImportHDF5::loadInfo (H5File file, std::shared_ptr<Project> proj) {
    try {
        MessageRelay::emitUpdateDetailMax(3);

        DataSet coordinate_format = file.openDataSet("/coordinate_format");
        {
            std::string cf;
            DataType dt = coordinate_format.getDataType();
            coordinate_format.read(cf,dt);

            std::shared_ptr<Project::CoordinateSystemInfo> csi = std::shared_ptr<Project::CoordinateSystemInfo>(new Project::CoordinateSystemInfo());

            if (cf.compare("Cartesian") == 0) {
                csi->setCoordinateSystemType(Project::CoordinateSystemInfo::CoordinateSystemType::CST_CARTESIAN);
                /* now get the dimensions */
                Group testSlice = file.openGroup("images/frames/0/slices/0");
                auto ret = readMultipleValues<uint32_t>(testSlice, "dimensions");
                if (std::get<2>(ret) != 1)
                    throw CTFormatException("hyperdimensional images?");
                if (*std::get<1>(ret) != 2)
                    throw CTFormatException("currently only two dimensional images are supported");
                uint32_t *dims = std::get<0>(ret);
                Project::CoordinateSystemInfo::CoordinateSystemData csd = { dims[0], dims[1] };
                csi->setCoordinateSystemData(csd);

                delete[] std::get<0>(ret);
                delete[] std::get<1>(ret);
            } else {
                throw CTFormatException("Unsupported coordinate format "+cf);
            }

            proj->setCoordinateSystemInfo(csi);
        }
        MessageRelay::emitIncreaseDetail();

        Group info = file.openGroup("info");
        {
            /*! \todo inputFiles don't work yet */
            QList<std::string> files;
            files.append("inputFiles cannot be parsed yet.");
            proj->getInfo()->setInputFiles(files);
        }
        MessageRelay::emitIncreaseDetail();

        {
            std::string time;
            DataSet timeOfConversion = info.openDataSet("timeOfConversion");
            DataType datatype = timeOfConversion.getDataType();

            timeOfConversion.read(time,datatype);
            QDateTime dateTime = QDateTime::fromString(time.c_str(), "dd-MM-yyyy-hh:mm:ss");
            proj->getInfo()->setTimeOfConversion(dateTime);
        }
        MessageRelay::emitIncreaseDetail();
    } catch (H5::GroupIException &e) {
        throw CTFormatException ("Format mismatch while trying to read info: " + e.getDetailMsg());
    }

    return true;
}

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

    Group annotationElement (H5Gopen(group_id,name,H5P_DEFAULT));
    StrType st(PredType::C_S1, H5T_VARIABLE);
    uint32_t id = readSingleValue<uint32_t>(annotationElement, "track_annotation_id");
    char *title = readSingleValue<char*>(annotationElement, "title");
    char *description = readSingleValue<char*>(annotationElement, "description");

    QString t(title);
    QString d(description);
    std::shared_ptr<Annotation> a = std::shared_ptr<Annotation>(new Annotation(Annotation::TRACKLET_ANNOTATION, id, t, d));
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
    Group annotationElement (H5Gopen(group_id,name,H5P_DEFAULT));
    uint32_t id = readSingleValue<uint32_t>(annotationElement, "object_annotation_id");
    char *title = readSingleValue<char*>(annotationElement,"title");
    char *description = readSingleValue<char*>(annotationElement,"description");

    QString t(title);
    QString d(description);
    std::shared_ptr<Annotation> a = std::shared_ptr<Annotation>(new Annotation(Annotation::OBJECT_ANNOTATION, id, t, d));
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
            MessageRelay::emitUpdateDetailMax(totalSize);

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

/*!
 * \brief converts a unit8_t[][][] into a QImage
 * \param buf the buffer that holds the image
 * \param height height of the image in pixels
 * \param width width of the image in pixels
 * \param depth depth of the image (1 if grayscale, 3 if rgb)
 * \return a std::shared_ptr<QImage>, that points to the image
 */
std::shared_ptr<QImage> ImportHDF5::bufToImage (uint8_t *buf, hsize_t height, hsize_t width, hsize_t depth) {
    int offy = width*depth;
    int offx = depth;

    std::shared_ptr<QImage> img(new QImage(width,height,QImage::Format_RGB32));
    for (unsigned int posy=0; posy<height; posy++) {
        /*! \todo use QImage::scanLine instead of QImage::scanPixel */
        for (unsigned int posx=0; posx<width; posx++) {
            QColor col;
            if(depth == 3) {
                uint8_t r = buf[posy * offy + posx * offx + 0];
                uint8_t g = buf[posy * offy + posx * offx + 1];
                uint8_t b = buf[posy * offy + posx * offx + 2];
                col.setRgb(r,g,b);
            } else {
                uint8_t c = buf[posy * offy + posx * offx];
                col.setRgb(c,c,c);
            }
            img->setPixel(posx,posy,col.rgb());
        }
    }

    return img;
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
    if (rank == 3) {
        img = bufToImage(buf, dims[0], dims[1], dims[2]);
    } else {
        img = bufToImage(buf, dims[0], dims[1], 1);
    }

    delete[] dims;
    delete[] buf;

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
    Slice *slice = static_cast<Slice*>(op_data);

    if (statbuf.type == H5G_DATASET) {
        std::string sname(name);
        if(std::all_of(sname.begin(),sname.end(),::isdigit)) {
            int channr = atoi(name);
            std::shared_ptr<Channel> channel = slice->getChannel(channr);

            if(channel == nullptr) {
                channel = std::shared_ptr<Channel>(new Channel(channr, slice->getSliceId(), slice->getFrameId()));
                slice->addChannel(channel);
            }

            auto data = readMultipleValues<uint8_t>(H5Dopen(group_id, name, H5P_DEFAULT));
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
        Group group(H5Gopen(group_id, name, H5P_DEFAULT));
        int slicenr = readSingleValue<int>(group, "slice_id");

        std::shared_ptr<Slice> slice = frame->getSlice(slicenr);
        if (slice == nullptr) {
            slice = std::shared_ptr<Slice>(new Slice(slicenr, frame->getID()));
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
        Group frameGroup (H5Gopen(group_id, name, H5P_DEFAULT));
        int framenr = readSingleValue<int>(frameGroup, "frame_id");

        /* Check if Frame exists. If it does, use this frame, else create one */
        std::shared_ptr<Frame> frame = movie->getFrame(framenr);
        if (frame == nullptr) {
            frame = std::shared_ptr<Frame>(new Frame(framenr));
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
    std::shared_ptr<QPoint> point(new QPoint());

    auto data = readMultipleValues<uint16_t>(H5Dopen(objGroup,"centroid",H5P_DEFAULT));
    uint16_t *buf = std::get<0>(data);

    point->setX(buf[0]);
    point->setY(buf[1]);

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
    std::shared_ptr<QRect> box (new QRect());

    auto data = readMultipleValues<uint16_t>(H5Dopen(objGroup, "bounding_box", H5P_DEFAULT));
    uint16_t *buf = std::get<0>(data);

    std::shared_ptr<Project::CoordinateSystemInfo> csi = currentProject->getCoordinateSystemInfo();
    if (csi->getCoordinateSystemType() == Project::CoordinateSystemInfo::CoordinateSystemType::CST_CARTESIAN) {
        /* cartesian */
        uint32_t iW = csi->getCoordinateSystemData().imageWidth;
        uint32_t iH = csi->getCoordinateSystemData().imageHeight;

        box->setCoords(buf[0], iW - buf[3], buf[2], iH - buf[1]);
    } else if (csi->getCoordinateSystemType() == Project::CoordinateSystemInfo::CoordinateSystemType::CST_QTIMAGE){
        /* QT image */
        box->setCoords(buf[0], buf[1], buf[2], buf[3]);
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
    std::shared_ptr<QPolygonF> poly (new QPolygonF());

    auto data = readMultipleValues<uint32_t>(H5Dopen(objGroup, "outline", H5P_DEFAULT));
    uint32_t *buf = std::get<0>(data);
    hsize_t *dims = std::get<1>(data);

    hsize_t length = dims[0];

    for (hsize_t i = 0; i < length; i++) {
        poly->append(QPoint(buf[i*2],buf[i*2 + 1]));
    }
    /* Close the polygon */
    poly->append(QPoint(buf[0],buf[1]));

    delete[] (buf);
    delete[] (dims);

    return poly;
}

/*!
 * \brief Callback for iterating over /objects/frames/slices/channels/objects
 * \param group_id callback parameter
 * \param name callback parameter
 * \param op_data callback parameter, holds a pointer to an Object
 * \return callback status
 */
herr_t ImportHDF5::process_objects_frames_slices_channels_objects (hid_t group_id, const char *name, void *op_data) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    herr_t err = 0;
    Channel *cptr = static_cast<Channel *> (op_data);

    if (statbuf.type == H5G_GROUP) {
        Group objGroup (H5Gopen(group_id, name, H5P_DEFAULT));
        uint32_t objNr = readSingleValue<uint32_t>(objGroup,"object_id");

        std::shared_ptr<Object> object = cptr->getObject(objNr);

        if (!object) {
            object = std::shared_ptr<Object> (new Object(objNr, cptr->getChanId(), cptr->getSliceId(), cptr->getFrameId()));
            cptr->addObject(object);
        }

        std::shared_ptr<QRect> bbox = readBoundingBox(objGroup.getId());
        object->setBoundingBox(bbox);
        std::shared_ptr<QPoint> centroid = readCentroid(objGroup.getId());
        object->setCentroid(centroid);
        std::shared_ptr<QPolygonF> outline = readOutline(objGroup.getId());
        object->setOutline(outline);

        std::shared_ptr<QPolygonF> nOutline = std::shared_ptr<QPolygonF>(new QPolygonF());
        for (QPointF &p : *object->getOutline()) {
            double x = p.x();
            double y = 250 - p.y();
            nOutline->append(QPointF(x,y));
        }
        object->setOutline(nOutline);
    }

    return err;
}

/*!
 * \brief Callback for iterating over /objects/frames/slices/channels
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
        Group channelGrp(H5Gopen(group_id, name, H5P_DEFAULT));
        int chanNr = readSingleValue<int>(channelGrp, "channel_id");
        std::shared_ptr<Channel> channel = sptr->getChannel(chanNr);

        if (!channel) {
            channel = std::shared_ptr<Channel> (new Channel(chanNr, sptr->getSliceId(), sptr->getFrameId()));
            sptr->addChannel(channel);
        }

        Group cGroup(H5Gopen(group_id, name, H5P_DEFAULT));
        err = H5Giterate(cGroup.getId(), "objects", NULL, process_objects_frames_slices_channels_objects, &(*channel));
    }

    return err;
}

/*!
 * \brief Callback for iterating over /objects/frames/slices
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
        Group sliceGrp(H5Gopen(group_id, name, H5P_DEFAULT));
        int sliceNr = readSingleValue<int>(sliceGrp, "slice_id");
        std::shared_ptr<Slice>  slice = fptr->getSlice(sliceNr);

        if (!slice) {
            slice = std::shared_ptr<Slice> (new Slice(sliceNr, fptr->getID()));
            fptr->addSlice(slice);
        }

        std::string sName(name);
        sName.append("/channels");
        err = H5Giterate(group_id, sName.c_str(), NULL, process_objects_frames_slices_channels, &(*slice));
    }

    return err;
}

/*!
 * \brief Callback for iterating over /objects/frames
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
        Group frameGrp(H5Gopen(group_id, name, H5P_DEFAULT));
        int frameNr = readSingleValue<int>(frameGrp, "frame_id");
        std::shared_ptr<Frame> frame = mptr->getFrame(frameNr);

        if (!frame) {
            frame = std::shared_ptr<Frame> (new Frame(frameNr));
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
            MessageRelay::emitUpdateDetailMax(getGroupSize(objects.getId(),"frames"));
            err = H5Giterate(objects.getId(), "frames", NULL, process_objects_frames, &(*movie));
        } catch (H5::GroupIException &e) {
            throw CTFormatException ("Format mismatch while trying to read objects: " + e.getDetailMsg());
        }
    }
    return !err;
}

/*!
 * \brief Callback for iterating over /tracklets/objects
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
        Group objGroup(H5Gopen(group_id, name, H5P_DEFAULT));

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
            autotracklet->addComponent(frame,object);
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

    Group daughter(H5Gopen(group_id, name, H5P_DEFAULT));
    int dId = readSingleValue<int>(daughter, "autotracklet_id");
    names->push_back(dId);

    return 0;
}

/*!
 * \brief Callback for iterating over the mother-daughter relations of /tracklets
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
    Group group(H5Gopen(group_id_o, name, H5P_DEFAULT));

    if (statbuf.type == H5G_GROUP) {
        herr_t err;
        uint32_t atId = readSingleValue<uint32_t>(group, "autotracklet_id");
        std::shared_ptr<AutoTracklet> at = project->getAutoTracklet(atId);

        if (linkExists(group, "next_event") && linkExists(group, "next")) {
            /* get event type */
            Group nextEv = group.openGroup("next_event");

            char *evName = readSingleValue<char*>(nextEv, "name");
            std::string sEvName(evName);
            if (sEvName.compare("cell_division") == 0) {
                std::shared_ptr<TrackEventDivision<AutoTracklet>> ted =
                        std::shared_ptr<TrackEventDivision<AutoTracklet>>(new TrackEventDivision<AutoTracklet>());
                ted->setPrev(at);
                std::list<int> nextIds;
                err = H5Giterate(group.getId(), "next", NULL, process_autotracklets_events_ids, &nextIds);
                std::shared_ptr<QList<std::shared_ptr<AutoTracklet>>> nList =
                        std::shared_ptr<QList<std::shared_ptr<AutoTracklet>>>(new QList<std::shared_ptr<AutoTracklet>>());

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
                qDebug() << "unhandled event in autotracklet" << name << "L:" << evName;
            }
        }
    }

    MessageRelay::emitIncreaseDetail();
    return 0;
}

/*!
 * \brief Callback for iterating over /autotracklets
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
        Group trackGroup (H5Gopen(group_id, name, H5P_DEFAULT));
        int atnr = readSingleValue<int>(trackGroup, "autotracklet_id");

        std::shared_ptr<AutoTracklet> autoTracklet = project->getAutoTracklet(atnr);

        if (!autoTracklet) {
            autoTracklet = std::shared_ptr<AutoTracklet>(new AutoTracklet());
            autoTracklet->setID(atnr);
            project->addAutoTracklet(autoTracklet);
        }

        std::pair<std::shared_ptr<AutoTracklet>,Project*> p(autoTracklet,project);
        /* add the objects to this autotracklet */
        err = H5Giterate(trackGroup.getId(), "objects", NULL, process_autotracklets_objects, &(p));
    }

    MessageRelay::emitIncreaseDetail();
    return err;
}

herr_t ImportHDF5::process_tracklets_events_ids(hid_t group_id, const char *name, void *opdata) {
    std::list<int> *names = static_cast<std::list<int>*>(opdata);

    Group daughter(H5Gopen(group_id, name, H5P_DEFAULT));
    int dId = readSingleValue<int>(daughter, "tracklet_id");
    names->push_back(dId);

    return 0;
}

herr_t ImportHDF5::process_tracklets_events(hid_t group_id_o, const char *name, void *opdata) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id_o, name, true, &statbuf);
    Project *project = static_cast<Project*> (opdata);
    Group group(H5Gopen(group_id_o, name, H5P_DEFAULT));

    if (statbuf.type == H5G_GROUP) {
        herr_t err;
        uint32_t atId = readSingleValue<uint32_t>(group, "tracklet_id");
        std::shared_ptr<Tracklet> tracklet = project->getGenealogy()->getTracklet(atId);

        if (linkExists(group, "next_event") && linkExists(group, "next")) {
            /* get event type */
            Group nextEv = group.openGroup("next_event");

            char *evName = readSingleValue<char*>(nextEv, "name");
            std::string sEvName(evName);
            if (sEvName.compare("cell_division") == 0) {
                std::shared_ptr<TrackEventDivision<Tracklet>> ted =
                        std::shared_ptr<TrackEventDivision<Tracklet>>(new TrackEventDivision<Tracklet>());
                ted->setPrev(tracklet);
                std::list<int> nextIds;
                err = H5Giterate(group.getId(), "next", NULL, process_tracklets_events_ids, &nextIds);
                std::shared_ptr<QList<std::shared_ptr<Tracklet>>> nList =
                        std::shared_ptr<QList<std::shared_ptr<Tracklet>>>(new QList<std::shared_ptr<Tracklet>>());

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
            } else {
                qDebug() << "unhandled event in autotracklet" << name << "L:" << evName;
            }
        }
    }

    MessageRelay::emitIncreaseDetail();
    return 0;
}

herr_t ImportHDF5::process_tracklets_objects(hid_t group_id, const char *name, void *opdata) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    std::pair<std::shared_ptr<Tracklet>,Project*>* p = static_cast<std::pair<std::shared_ptr<Tracklet>,Project*>*>(opdata);
    std::shared_ptr<Tracklet> tracklet = p->first;
    Project *project = p->second;

    if (statbuf.type == H5G_GROUP) {
        Group objGroup(H5Gopen(group_id, name, H5P_DEFAULT));

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


herr_t ImportHDF5::process_tracklets (hid_t group_id, const char *name, void *op_data) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    herr_t err = 0;
    Project *project = static_cast<Project*>(op_data);

    if (statbuf.type == H5G_GROUP) {
        Group trackGroup (H5Gopen(group_id, name, H5P_DEFAULT));
        int atnr = readSingleValue<int>(trackGroup, "tracklet_id");

        std::shared_ptr<Tracklet> tracklet = project->getGenealogy()->getTracklet(atnr);

        if (!tracklet) {
            tracklet = std::shared_ptr<Tracklet>(new Tracklet());
            tracklet->setId(atnr);
            project->getGenealogy()->addTracklet(tracklet);
        }

        std::pair<std::shared_ptr<Tracklet>,Project*> p(tracklet,project);
        /* add the objects to this tracklet */
        err = H5Giterate(trackGroup.getId(), "objects", NULL, process_tracklets_objects, &(p));
    }

    MessageRelay::emitIncreaseDetail();
    return err;
}

/*!
 * \brief loads Tracklet%s for a Project from a given HDF5 file
 * \param file the file that is read from
 * \param proj the Project into which the Tracklet%s are read
 * \return true, if everything went fine, false otherwise
 * \throw CTFormatException if iterating over the elements failed
 */
bool ImportHDF5::loadAutoTracklets(H5File file, std::shared_ptr<Project> proj) {
    herr_t err = 0;

    try {
        MessageRelay::emitUpdateDetailMax(getGroupSize(file.getId(),"autotracklets"));
        err = H5Giterate(file.getId(), "autotracklets", NULL, process_autotracklets, &(*proj));
    } catch (H5::GroupIException &e) {
        throw CTFormatException ("Format mismatch while trying to read autotracklets: " + e.getDetailMsg());
    }

    return !err;
}

bool ImportHDF5::loadTracklets(H5File file, std::shared_ptr<Project> proj)
{
    herr_t err = 0;

    try {
        if (groupExists(file, "tracklets")) {
            MessageRelay::emitUpdateDetailMax(getGroupSize(file.getId(),"tracklets"));
            err = H5Giterate(file.getId(), "tracklets", NULL, process_tracklets, &(*proj));
        }
    } catch (H5::GroupIException &e) {
        throw CTFormatException ("Format mismatch while trying to read tracklets: " + e.getDetailMsg());
    }

    return !err;
}

bool ImportHDF5::loadEventInstances(H5File file, std::shared_ptr<Project> proj) {
    herr_t err = 0;

    try {
        int total = getGroupSize(file.getId(), "autotracklets");
        if (groupExists(file, "tracklets"))
            total += getGroupSize(file.getId(), "tracklets");
        MessageRelay::emitUpdateDetailMax(total);
        err = H5Giterate(file.getId(), "autotracklets", NULL, process_autotracklets_events, &(*proj));
        if (groupExists(file, "tracklets"))
            err = err && H5Giterate(file.getId(), "tracklets", NULL, process_tracklets_events, &(*proj));
    } catch (H5::GroupIException &e) {
        throw CTFormatException ("Format mismatch while trying to read mother-daughter relations: " + e.getDetailMsg());
    }

    return !err;
}


bool Validator::test_groupname_matches_object_id(H5File file, checkObject checkee, std::string prefix, std::string &err) {
    std::string groupName = checkee.name;
    Group objectGrp = file.openGroup(prefix + "/" + groupName);
    int objectId = readSingleValue<int>(objectGrp, "object_id");
    if (std::atoi(groupName.c_str()) == objectId)
        return true;
    err = "object_id " + std::to_string(objectId) + " mismatches groupname "+groupName;
    return false;
}

bool Validator::test_groupname_matches_channel_id(H5File file, checkObject checkee, std::string prefix, std::string &err) {
    std::string groupName = checkee.name;
    Group objectGrp = file.openGroup(prefix + "/" + groupName);
    int objectId = readSingleValue<int>(objectGrp, "channel_id");
    if (std::atoi(groupName.c_str()) == objectId)
        return true;
    err = "channel_id " + std::to_string(objectId) + " mismatches groupname "+groupName;
    return false;
}

bool Validator::test_groupname_matches_slice_id(H5File file, checkObject checkee, std::string prefix, std::string &err) {
    std::string groupName = checkee.name;
    Group objectGrp = file.openGroup(prefix + "/" + groupName);
    int objectId = readSingleValue<int>(objectGrp, "slice_id");
    if (std::atoi(groupName.c_str()) == objectId)
        return true;
    err = "slice_id " + std::to_string(objectId) + " mismatches groupname "+groupName;
    return false;
}

bool Validator::test_groupname_matches_frame_id(H5File file, checkObject checkee, std::string prefix, std::string &err) {
    std::string groupName = checkee.name;
    Group objectGrp = file.openGroup(prefix + "/" + groupName);
    int objectId = readSingleValue<int>(objectGrp, "frame_id");
    if (std::atoi(groupName.c_str()) == objectId)
        return true;
    err = "frame_id " + std::to_string(objectId) + " mismatches groupname "+groupName;
    return false;
}

bool Validator::test_groupname_matches_tracklet_id(H5File file, checkObject checkee, std::string prefix, std::string &err) {
    std::string groupName = checkee.name;
    Group objectGrp = file.openGroup(prefix + "/" + groupName);
    int objectId = readSingleValue<int>(objectGrp, "tracklet_id");
    if (std::atoi(groupName.c_str()) == objectId)
        return true;
    err = "tracklet_id " + std::to_string(objectId) + " mismatches groupname "+groupName;
    return false;
}

bool Validator::test_groupname_matches_autotracklet_id(H5File file, checkObject checkee, std::string prefix, std::string &err) {
    std::string groupName = checkee.name;
    Group objectGrp = file.openGroup(prefix + "/" + groupName);
    int objectId = readSingleValue<int>(objectGrp, "autotracklet_id");
    if (std::atoi(groupName.c_str()) == objectId)
        return true;
    err = "autotracklet_id " + std::to_string(objectId) + " mismatches groupname "+groupName;
    return false;
}
/*!
 * \brief ImportHDF5::validCellTrackerFile
 * Checks if a given file is a valid CellTracker project file and coheres to certain basic constraints.
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
                                  {"object_annotation_id", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}},
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
                                {"slice_shape", true, H5L_TYPE_HARD, TYPE_DATASET, nullptr, {}}}},
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
