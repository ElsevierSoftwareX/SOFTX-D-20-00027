#include "importhdf5.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <tuple>

#include <QColor>
#include <QDateTime>
#include <QDebug>
#include <QList>
#include <QLocale>
#include <QPolygonF>
#include <QPoint>
#include <QRect>

#include "corrected_data/trackeventdivision.h"
#include "exceptions/ctimportexception.h"
#include "exceptions/ctformatexception.h"
#include "exceptions/ctmissingelementexception.h"
#include "provider/messagerelay.h"

namespace CellTracker {
using namespace H5;

ImportHDF5::ImportHDF5()
{
}

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

        MessageRelay::emitUpdateOverallName("Importing from HDF5");
        MessageRelay::emitUpdateOverallMax(4);

        proj = setupEmptyProject();
//        qDebug() << "Not loading info";
//        if (!loadInfo(file,proj))
//            throw CTImportException ("Loading the project information failed");
//        qDebug() << "Not loading images";
//        if (!loadImages(file,proj))
//            throw CTImportException ("Loading the images failed.");
        qDebug() << "Loading objects";
        if (!loadObjects(file,proj))
            throw CTImportException ("Loading the objects failed.");
        MessageRelay::emitIncreaseOverall();

        qDebug() << "Loading tracklets";
        if (!loadTracklets(file, proj))
            throw CTImportException ("Loading the tracklets failed.");
        MessageRelay::emitIncreaseOverall();

        qDebug() << "Loading Mother-Daughter relations";
        if (!loadDaughterRelations(file, proj))
            throw CTImportException ("Loading the mother-daughter relations failed.");
        MessageRelay::emitIncreaseOverall();

        qDebug() << "Loading annotations";
        if (!loadAnnotations(file,proj))
            throw CTImportException ("Loading the Annotations failed.");
        MessageRelay::emitIncreaseOverall();

        qDebug() << "Finished";
    } catch (H5::FileIException &e) {
        throw CTImportException ("Opening the file " + fileName.toStdString() + " failed: " + e.getDetailMsg());
    }

    return proj;
}

template <typename T> inline T readSingleValue(DataSet dset) {
    T ret;
    DataType dtype = dset.getDataType();

    dset.read(&ret, dtype);

    return ret;
}

template <typename T> inline T readSingleValue(hid_t dset_id) {
    return readSingleValue<T>(DataSet(dset_id));
}

template <typename T> inline T readSingleValue(Group group, const char *name) {
    T ret;
    DataSet dset = group.openDataSet(name);

    ret = readSingleValue<T>(dset);

    return ret;
}

template <typename T> inline T readSingleValue(hid_t group_id, const char *name) {
    return readSingleValue<T>(Group(group_id), name);
}

template <typename T> inline std::tuple<T *,hsize_t *,int> readMultipleValues(DataSet dset) {
    DataType dtype = dset.getDataType();
    DataSpace dspace = dset.getSpace();

    /* Resize the buffer, so all the Elements fit in */
    int rank = dspace.getSimpleExtentNdims();
    hsize_t *dims = new hsize_t[rank];
    dspace.getSimpleExtentDims(dims);
    T *buf;
    hsize_t size = 1;
    for (int i = 0; i < rank; i++)
        size *= dims[i];
    buf = new T[size];

    dset.read(buf,dtype);

    return std::make_tuple(buf,dims,rank);
}

template <typename T> inline std::tuple<T *,hsize_t *,int> readMultipleValues(hid_t dset_id) {
    return readMultipleValues<T>(DataSet (dset_id));
}

template <typename T> inline std::tuple<T *,hsize_t *,int> readMultipleValues(Group group, const char *name) {
    DataSet dset = group.openDataSet(name);

    auto ret = readMultipleValues<T>(dset);

    return ret;
}

template <typename T> inline std::tuple<T *, hsize_t *, int> readMultipleValues(hid_t group_id, const char *name) {
    return readMultipleValues<T>(Group(group_id), name);
}

hsize_t getGroupSize(hid_t gid, const char *name) {
    H5G_info_t info;
    htri_t ret;

    ret = H5Lexists(gid, name, H5P_DEFAULT);

    if(ret >= 0 && ret == true) {
        hid_t group = H5Gopen(gid,name,H5P_DEFAULT);

        H5Gget_info(group,&info);
        H5Gclose(group);

        return info.nlinks;
    } else
        return 0;

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
        Group info = file.openGroup("info");
        {
            /*! \todo inputFiles don't work yet */
            QList<std::string> files;
            files.append("inputFiles cannot be parsed yet.");
            proj->getInfo()->setInputFiles(files);
        }
        {
            std::string time;
            DataSet timeOfConversion = info.openDataSet("timeOfConversion");
            DataType datatype = timeOfConversion.getDataType();

            timeOfConversion.read(time,datatype);
            QDateTime dateTime = QDateTime::fromString(time.c_str(), "dd-MM-yyyy-hh:mm:ss");
            proj->getInfo()->setTimeOfConversion(dateTime);
        }
        {
            Group trackingInfo = info.openGroup("tracking_info");
            {
                std::string algo;
                DataSet algorithm = trackingInfo.openDataSet("algorithm");
                DataType datatype = algorithm.getDataType();

                algorithm.read(algo,datatype);
                proj->getInfo()->setTrackingInfoAlgorithm(algo);
            }
            {
                std::string vers;
                DataSet version = trackingInfo.openDataSet("ilastik_version");
                DataType datatype = version.getDataType();

                version.read(vers, datatype);
                proj->getInfo()->setTrackingInfoILastikVersion(vers);
            }
            {
                std::string time;
                DataSet timeOfTracking = trackingInfo.openDataSet("timeOfTracking");
                DataType datatype = timeOfTracking.getDataType();

                timeOfTracking.read(time,datatype);
                QLocale enUS("en_US");
                QDateTime datetime = enUS.toDateTime(time.c_str(), "ddd MMM dd HH:mm:ss yyyy");
                proj->getInfo()->setTrackingInfoTimeOfTracking(datetime);
            }
        }
    } catch (H5::GroupIException &e) {
        throw CTFormatException ("Format mismatch while trying to read info: " + e.getDetailMsg());
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
    char *text = readSingleValue<char*>(annotationElement,"description");
    uint32_t id = readSingleValue<uint32_t>(annotationElement,"track/tracklet_id");

    std::shared_ptr<Tracklet> tracklet = gen->getTracklet(id);

    gen->addAnnotation(tracklet,std::string(text));

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
    char *text = readSingleValue<char*>(annotationElement,"description");

    uint32_t fid = readSingleValue<uint32_t>(annotationElement,"object/frame_id");
    uint32_t sid = readSingleValue<uint32_t>(annotationElement,"object/slice_id");
    uint32_t oid = readSingleValue<uint32_t>(annotationElement,"object/id");

    std::shared_ptr<Object> object = gen->getObjectAt(fid, sid, oid);

    gen->addAnnotation(object,std::string(text));

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
            MessageRelay::emitUpdateDetailName("Loading annotations");
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
std::shared_ptr<QImage> bufToImage (uint8_t *buf, hsize_t height, hsize_t width, hsize_t depth) {
    int offy = width*depth;
    int offx = depth;

    std::shared_ptr<QImage> img(new QImage(width,height,QImage::Format_RGB32));
    for (unsigned int posy=0; posy<height; posy++) {
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
    Group frameGroup = framesGroup.openGroup(std::to_string(frame).c_str());
    Group sliceGroup = frameGroup.openGroup(std::to_string(slice).c_str());

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
                channel = std::shared_ptr<Channel>(new Channel(channr));
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
        int slicenr = atoi(name);

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
        int framenr = readSingleValue<int>(frameGroup, "id");

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
std::shared_ptr<QPoint> readCentroid(hid_t objGroup) {
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
std::shared_ptr<QRect> readBoundingBox(hid_t objGroup) {
    std::shared_ptr<QRect> box (new QRect());

    auto data = readMultipleValues<uint16_t>(H5Dopen(objGroup, "bounding_box", H5P_DEFAULT));
    uint16_t *buf = std::get<0>(data);

    box->setCoords(buf[0], buf[1], buf[2], buf[3]);

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
std::shared_ptr<QPolygonF> readOutline (hid_t objGroup) {
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
 * \brief reads properties of /objects/frames/slices/objects
 * \param group_id callback parameter
 * \param name callback parameter
 * \param op_data callback parameter, holds a pointer to an Object
 * \return callback status
 */
herr_t ImportHDF5::process_objects_frames_slices_objects_properties(hid_t group_id, const char *name, void *op_data) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    Object *optr = static_cast<Object*>(op_data);

    if (statbuf.type == H5G_DATASET) {
        std::string sname(name);

        if (!sname.compare("bounding_box")) {
            std::shared_ptr<QRect> bbox = readBoundingBox(group_id);
            optr->setBoundingBox(bbox);
        } else if (!sname.compare("centroid")) {
            std::shared_ptr<QPoint> centroid = readCentroid(group_id);
            optr->setCentroid(centroid);
        } else if (!sname.compare("outline")) {
            std::shared_ptr<QPolygonF> outline = readOutline(group_id);
            optr->setOutline(outline);
        }
    }

    return 0;
}

/*!
 * \brief Callback for iterating over /objects/frames/slices/objects
 * \param group_id callback parameter
 * \param name callback parameter
 * \param op_data callback parameter, holds a pointer to an Object
 * \return callback status
 */
herr_t ImportHDF5::process_objects_frames_slices_objects (hid_t group_id, const char *name, void *op_data) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    herr_t err = 0;
    Slice *sptr = static_cast<Slice *> (op_data);

    if (statbuf.type == H5G_GROUP) {
        Group objGroup (H5Gopen(group_id, name, H5P_DEFAULT));
        uint32_t objNr = readSingleValue<uint32_t>(objGroup,"id");

        std::shared_ptr<Object> object = sptr->getObject(objNr);

        if (!object) {
            object = std::shared_ptr<Object> (new Object(objNr, sptr->getId(), sptr->getFrameId()));
            sptr->addObject(object);
        }

        err = H5Giterate(group_id, name, NULL, process_objects_frames_slices_objects_properties, &(*object));

        /*! \todo the polygon gets mirrored here. remove when fixed in the data format */
        QPoint tl = object->getBoundingBox()->topLeft();
        std::shared_ptr<QPolygonF> nOutline = std::shared_ptr<QPolygonF>(new QPolygonF());
        for (QPointF &p : *object->getOutline()) {
            double x = tl.x() - tl.y() + p.y();
            double y = tl.y() - tl.x() + p.x();
            nOutline->append(QPointF(x,y));
        }
        object->setOutline(nOutline);
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
        int sliceNr = atoi(name);
        std::shared_ptr<Slice>  slice = fptr->getSlice(sliceNr);

        if (!slice) {
            slice = std::shared_ptr<Slice> (new Slice(sliceNr, fptr->getID()));
            fptr->addSlice(slice);
        }

        err = H5Giterate(group_id, name, NULL, process_objects_frames_slices_objects, &(*slice));
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
        int frameNr = atoi (name);
        std::shared_ptr<Frame> frame = mptr->getFrame(frameNr);

        if (!frame) {
            frame = std::shared_ptr<Frame> (new Frame(frameNr));
            mptr->addFrame(frame);
        }

        err = H5Giterate(group_id, name, NULL, process_objects_frames_slices, &(*frame));
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
            MessageRelay::emitUpdateDetailName("Loading Objects");
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
herr_t ImportHDF5::process_tracklets_objects(hid_t group_id, const char *name, void *opdata) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    Project *project = static_cast<Project*> (opdata);

    if (statbuf.type == H5G_GROUP) {
        Group objGroup(H5Gopen(group_id, name, H5P_DEFAULT));

        uint32_t objId = readSingleValue<uint32_t>(objGroup, "id");
        uint32_t frameId = readSingleValue<uint32_t>(objGroup, "frame_id");
        uint32_t sliceId = readSingleValue<uint32_t>(objGroup, "slice_id");
        int autoId = readSingleValue<int>(H5Dopen(group_id, "track_id", H5P_DEFAULT));

        std::shared_ptr<AutoTracklet> autoTracklet = project->getAutoTracklet(autoId);
        std::shared_ptr<Frame> frame = project->getMovie()->getFrame(frameId);
        std::shared_ptr<Object> object = frame->getSlice(sliceId)->getObject(objId);

        if (frame == nullptr)
            throw CTMissingElementException("Did not find frame " + std::to_string(frameId) + " in Movie");
        if (autoTracklet == nullptr)
            throw CTMissingElementException("Did not find tracklet " + std::to_string(autoId) + " in genealogy");
        if (object == nullptr)
            throw CTMissingElementException("Did not find object " + std::to_string(objId) + " in slice " + std::to_string(sliceId) + " of frame " + std::to_string(frameId));

        if (autoTracklet != nullptr && object != nullptr && frame != nullptr) {
            autoTracklet->addComponent(frame,object);
        } else {
            throw CTMissingElementException("Error while adding object " + std::to_string(objId)
                    + " at frame " + std::to_string(frameId)
                    + "to track" + std::to_string(autoId));
        }
    }

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
herr_t ImportHDF5::process_tracklets_daughters(hid_t group_id_o, const char *name, void *opdata) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id_o, name, true, &statbuf);
    Project *project = static_cast<Project*> (opdata);
    Group group(H5Gopen(group_id_o, name, H5P_DEFAULT));

    if (statbuf.type == H5G_GROUP) {
        int trackId = readSingleValue<int>(H5Dopen(group.getId(), "track_id", H5P_DEFAULT));
        std::shared_ptr<QList<std::shared_ptr<AutoTracklet>>> daughters = std::shared_ptr<QList<std::shared_ptr<AutoTracklet>>>(new QList<std::shared_ptr<AutoTracklet>>());

        {
            // Get daughters
            htri_t ret = H5Lexists(group.getId(), "daughters", H5P_DEFAULT);
            if (ret >= 0 && ret == true) {
                auto data = readMultipleValues<uint32_t>(H5Dopen(group.getId(), "daughters", H5P_DEFAULT));
                uint32_t *buf = std::get<0>(data);
                hsize_t *dims = std::get<1>(data);
                int rank = std::get<2>(data);

                if (rank == 1){
                    for (hsize_t i = 0; i < dims[0]; i++) {
                        int idx = static_cast<int>(buf[i]);
                        std::shared_ptr<AutoTracklet> daughter = project->getAutoTracklet(idx);
                        if (daughter)
                            daughters->append(daughter);
                        else {
                            qDebug() << "Did not find Daughter-Tracklet"
                                     << idx << "in genealogy, which is required by track"
                                     << trackId << "(group" << name << ")";
                        }
                    }
                }

                delete[] (buf);
                delete[] (dims);
            }
        }

        std::shared_ptr<AutoTracklet> tracklet = project->getAutoTracklet(trackId);

        if (tracklet == nullptr)
            throw CTMissingElementException("Did not find tracklet " + std::to_string(trackId) + " in genealogy");
        if (!daughters->isEmpty() && tracklet != nullptr) {
            std::shared_ptr<TrackEventDivision<AutoTracklet>> event = std::shared_ptr<TrackEventDivision<AutoTracklet>>(new TrackEventDivision<AutoTracklet>());
            event->setNext(daughters);
            tracklet->setNext(event);
        }
    }

    MessageRelay::emitIncreaseDetail();
    return 0;
}

/*!
 * \brief Callback for iterating over /tracks
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
        Group trackGroup (H5Gopen(group_id, name, H5P_DEFAULT));
        int tracknr = readSingleValue<int>(trackGroup, "track_id");

        std::shared_ptr<AutoTracklet> autoTracklet = project->getAutoTracklet(tracknr);

        if (!autoTracklet) {
            autoTracklet = std::shared_ptr<AutoTracklet>(new AutoTracklet());
            autoTracklet->setID(tracknr);
            project->addAutoTracklet(autoTracklet);
        }

        err = H5Giterate(group_id, name, NULL, process_tracklets_objects, &(*project));

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
bool ImportHDF5::loadTracklets(H5File file, std::shared_ptr<Project> proj) {
    herr_t err = 0;

    try {
        MessageRelay::emitUpdateDetailName("Loading tracklets");
        MessageRelay::emitUpdateDetailMax(getGroupSize(file.getId(),"tracks"));
        err = H5Giterate(file.getId(), "tracks", NULL, process_tracklets, &(*proj));
    } catch (H5::GroupIException &e) {
        throw CTFormatException ("Format mismatch while trying to read tracklets: " + e.getDetailMsg());
    }

    return !err;
}

bool ImportHDF5::loadDaughterRelations(H5File file, std::shared_ptr<Project> proj) {
    herr_t err = 0;

    try {
        MessageRelay::emitUpdateDetailName("Loading mother-daughter relations");
        MessageRelay::emitUpdateDetailMax(getGroupSize(file.getId(),"tracks"));
        err = H5Giterate(file.getId(), "tracks", NULL, process_tracklets_daughters, &(*proj));
    } catch (H5::GroupIException &e) {
        throw CTFormatException ("Format mismatch while trying to read mother-daughter relations: " + e.getDetailMsg());
    }

    return !err;
}

}
