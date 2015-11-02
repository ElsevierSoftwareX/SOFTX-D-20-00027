#include "importhdf5.h"

#include <algorithm>
#include <cstdint>
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

/*!
file.h5                                                                                 CellTracker::ImportHDF5::load
├── annotations                                                                         CellTracker::ImportHDF5::loadAnnotations
│   ├── object_annotations                                                              "
│   │   ├── 0                                                                             \todo
│   │   │   ├── description                                                               \todo
│   │   │   ├── annotation_id                                                             \todo
│   │   │   └── title                                                                     \todo
│   │   ├── …                                                                           "
│   │   └── n                                                                           "
│   └── track_annotations                                                               "
│       ├── 0                                                                             \todo
│       │   ├── description                                                               \todo
│       │   ├── annotation_id                                                             \todo
│       │   └── title                                                                     \todo
│       ├── …                                                                           "
│       └── n                                                                           "
├── autotracklets                                                                       CellTracker::ImportHDF5::loadAutoTracklets
│   ├── 0                                                                                 CellTracker::ImportHDF5::process_autotracklets
│   │   ├── 0 -> /objects/frames/0/slices/0/channels/0/4                                    \todo
│   │   ├── 1 -> /objects/frames/1/slices/0/channels/0/6                                    \todo
│   │   ├── … -> /objects/frames/1/slices/0/channels/0/…                                    \todo
│   │   ├── n -> /objects/frames/1/slices/0/channels/0/n                                    \todo
│   │   ├── previous_event -> /events/cell division                                         \todo
│   │   ├── next_event -> /events/cell division                                             \todo
│   │   ├── next                                                                            \todo
│   │   │   ├── 0 -> /autotracklets/1                                                       \todo
│   │   │   └── 1 -> /autotracklets/2                                                       \todo
│   │   ├── previous                                                                        \todo
│   │   │   └── 0 -> /autotracklets/0                                                       \todo
│   │   │   └── 1 -> /autotracklets/2                                                       \todo
│   │   ├── start                                                                           \todo
│   │   ├── end                                                                             \todo
│   │   └── autotracklet_id                                                                 \todo
│   ├── …                                                                                 "
│   └── n                                                                                 "
├── coordinate_format                                                                   CellTracker::ImportHDF5::loadInfo
├── data_format_version                                                                 ignored
├── events                                                                              ignored, static
│   ├── cell dead                                                                       "
│   │   ├── description                                                                 "
│   │   ├── event_id                                                                    "
│   │   └── name                                                                        "
│   ├── cell division                                                                   "
│   │   └── …                                                                           "
│   ├── cell lost                                                                       "
│   │   └── …                                                                           "
│   ├── cell merge                                                                      "
│   │   └── …                                                                           "
│   ├── cell unmerge                                                                    "
│   │   └── …                                                                           "
│   └── end of movie                                                                    "
│       └── …                                                                           "
├── images                                                                              CellTracker::ImportHDF5::requestImage
│   ├── frames                                                                          "
│   │   ├── 0                                                                           "
│   │   │   ├── slices                                                                  "
│   │   │   │   ├── 0                                                                   "
│   │   │   │   │   ├── channels                                                        "
│   │   │   │   │   │   ├── 0                                                           "
│   │   │   │   │   │   ├── …                                                           "
│   │   │   │   │   │   └── n                                                           "
│   │   │   │   │   ├── dimensions                                                      "
│   │   │   │   │   ├── nchannels                                                       "
│   │   │   │   │   └── slice_id                                                        "
│   │   │   │   ├── …                                                                   "
│   │   │   │   └── n                                                                   "
│   │   │   └── frame_id                                                                "
│   │   ├── …                                                                           "
│   │   └── n                                                                           "
│   ├── frame_rate                                                                      "
│   ├── nframes                                                                         "
│   ├── nslices                                                                         "
│   └── slicing_shape                                                                   "
├── info                                                                                ignored
│   └── …                                                                               "
├── objects                                                                             CellTracker::ImportHDF5::loadObjects
│   ├── frames                                                                          "
│   │   ├── 0                                                                             CellTracker::ImportHDF5::process_objects_frames
│   │   │   ├── slices                                                                    "
│   │   │   │   ├── 0                                                                       CellTracker::ImportHDF5::process_objects_frames_slices
│   │   │   │   │   ├── channels                                                            "
│   │   │   │   │   │   ├── 0                                                                 CellTracker::ImportHDF5::process_objects_frames_slices_channels
│   │   │   │   │   │   │   ├── objects                                                       "
│   │   │   │   │   │   │   │   ├── 0                                                           CellTracker::ImportHDF5::process_objects_frames_slices_channels_objects
│   │   │   │   │   │   │   │   │   ├── bounding_box                                              CellTracker::ImportHDF5::process_objects_frames_slices_channels_objects_properties
│   │   │   │   │   │   │   │   │   ├── centroid                                                  "
│   │   │   │   │   │   │   │   │   ├── channel_id                                                "
│   │   │   │   │   │   │   │   │   ├── frame_id                                                  "
│   │   │   │   │   │   │   │   │   ├── slice_id                                                  "
│   │   │   │   │   │   │   │   │   ├── object_id                                                 "
│   │   │   │   │   │   │   │   │   ├── outline                                                   "
│   │   │   │   │   │   │   │   │   ├── packed_mask                                               "
│   │   │   │   │   │   │   │   │   └── annotations                                                 \todo
│   │   │   │   │   │   │   │   │       ├── 0 -> /annotations/object_annotations/2                  \todo
│   │   │   │   │   │   │   │   │       ├── … -> /annotations/object_annotations/…                  \todo
│   │   │   │   │   │   │   │   │       └── n -> /annotations/object_annotations/m                  \todo
│   │   │   │   │   │   │   │   ├── …                                                           CellTracker::ImportHDF5::process_objects_frames_slices_channels_objects
│   │   │   │   │   │   │   │   └── n                                                           CellTracker::ImportHDF5::process_objects_frames_slices_channels_objects
│   │   │   │   │   │   │   └── channel_id                                                    "
│   │   │   │   │   │   ├── …                                                                 CellTracker::ImportHDF5::process_objects_frames_slices_channels
│   │   │   │   │   │   └── n                                                                 CellTracker::ImportHDF5::process_objects_frames_slices_channels
│   │   │   │   │   ├── dimensions                                                          "
│   │   │   │   │   ├── nchannels                                                           "
│   │   │   │   │   └── slice_id                                                            "
│   │   │   │   ├── …                                                                       CellTracker::ImportHDF5::process_objects_frames_slices
│   │   │   │   └── n                                                                       CellTracker::ImportHDF5::process_objects_frames_slices
│   │   │   └── frame_id                                                                  "
│   │   ├── …                                                                             CellTracker::ImportHDF5::process_objects_frames
│   │   └── n                                                                             CellTracker::ImportHDF5::process_objects_frames
│   ├── frame_rate                                                                      "
│   ├── nframes                                                                         "
│   ├── nslices                                                                         "
│   └── slicing_shape                                                                   "
└── tracklets                                                                           CellTracker::ImportHDF5::loadTracklets
    ├── 0                                                                               \todo
    │   ├── 0 -> /objects/frames/0/slices/0/channels/0/4                                \todo
    │   ├── 1 -> /objects/frames/1/slices/0/channels/0/6                                \todo
    │   ├── … -> /objects/frames/1/slices/0/channels/0/…                                \todo
    │   ├── n -> /objects/frames/1/slices/0/channels/0/n                                \todo
    │   ├── previous_event -> /events/cell division                                     \todo
    │   ├── next_event -> /events/cell division                                         \todo
    │   ├── next                                                                        \todo
    │   │   ├── 0 -> /tracklets/1                                                       \todo
    │   │   └── 1 -> /tracklets/2                                                       \todo
    │   ├── previous                                                                    \todo
    │   │   ├── 0 -> /tracklets/0                                                       \todo
    │   │   └── 1 -> /tracklets/2                                                       \todo
    │   ├── annotations                                                                 \todo
    │   │   ├── 0 -> /annotations/track_annotations/1                                   \todo
    │   │   ├── … -> /annotations/track_annotations/…                                   \todo
    │   │   └── n -> /annotations/track_annotations/m                                   \todo
    │   ├── start                                                                       "
    │   ├── end                                                                         "
    │   └── tracklet_id                                                                 "
    ├── …                                                                               CellTracker::ImportHDF5::loadTracklets
    └── n                                                                               CellTracker::ImportHDF5::loadTracklets
*/

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
bool bla() {
    return true;
}

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
        std::list<std::pair<bool (*)(H5::H5File,std::shared_ptr<Project>),std::string>> phases = {
                {loadInfo,              "project information"},
                {loadObjects,           "objects"},
                {loadAutoTracklets,     "autotracklets"},
                {loadDaughterRelations, "mother-daughter relations"},
                {loadTracklets,         "tracklets"},
                {loadAnnotations,       "annotations"}
        };

        MessageRelay::emitUpdateOverallName("Importing from HDF5");
        MessageRelay::emitUpdateOverallMax(phases.size());

        proj = setupEmptyProject();
        currentProject = proj;

        qDebug() << "Importing from HDF5";
        for (std::pair<bool (*)(H5::H5File,std::shared_ptr<Project>),std::string> phase : phases) {
            bool (*function)(H5::H5File,std::shared_ptr<Project>) = phase.first;
            std::string text = "Loading " + phase.second;
            qDebug() << text.c_str();
            MessageRelay::emitUpdateDetailName(QString::fromStdString(text));
            if (!function(file, proj))
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
        /* group tracking_info has other information now */
//        {
//            Group trackingInfo = info.openGroup("tracking_info");
//            {
//                std::string algo;
//                DataSet algorithm = trackingInfo.openDataSet("algorithm");
//                DataType datatype = algorithm.getDataType();

//                algorithm.read(algo,datatype);
//                proj->getInfo()->setTrackingInfoAlgorithm(algo);
//            }
//            {
//                std::string vers;
//                DataSet version = trackingInfo.openDataSet("ilastik_version");
//                DataType datatype = version.getDataType();

//                version.read(vers, datatype);
//                proj->getInfo()->setTrackingInfoILastikVersion(vers);
//            }
//            {
//                std::string time;
//                DataSet timeOfTracking = trackingInfo.openDataSet("timeOfTracking");
//                DataType datatype = timeOfTracking.getDataType();

//                timeOfTracking.read(time,datatype);
//                QLocale enUS("en_US");
//                QDateTime datetime = enUS.toDateTime(time.c_str(), "ddd MMM dd HH:mm:ss yyyy");
//                proj->getInfo()->setTrackingInfoTimeOfTracking(datetime);
//            }
//        }
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
    uint32_t id = readSingleValue<uint32_t>(annotationElement, "id");
    char *title = readSingleValue<char*>(annotationElement, "title");
    char *description = readSingleValue<char*>(annotationElement, "description");

    QString t(title);
    QString d(description);
    std::shared_ptr<Annotation> a = std::shared_ptr<Annotation>(new Annotation(id, t, d));
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
    uint32_t id = readSingleValue<uint32_t>(annotationElement, "id");
    char *title = readSingleValue<char*>(annotationElement,"title");
    char *description = readSingleValue<char*>(annotationElement,"description");

    QString t(title);
    QString d(description);
    std::shared_ptr<Annotation> a = std::shared_ptr<Annotation>(new Annotation(id, t, d));
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
 * \brief reads properties of /objects/frames/slices/objects
 * \param group_id callback parameter
 * \param name callback parameter
 * \param op_data callback parameter, holds a pointer to an Object
 * \return callback status
 */
herr_t ImportHDF5::process_objects_frames_slices_channels_objects_properties(hid_t group_id, const char *name, void *op_data) {
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
        uint32_t objNr = readSingleValue<uint32_t>(objGroup,"id");

        std::shared_ptr<Object> object = cptr->getObject(objNr);

        if (!object) {
            object = std::shared_ptr<Object> (new Object(objNr, cptr->getChanId(), cptr->getSliceId(), cptr->getFrameId()));
            cptr->addObject(object);
        }

        err = H5Giterate(group_id, name, NULL, process_objects_frames_slices_channels_objects_properties, &(*object));

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
        int chanNr = atoi(name);
        std::shared_ptr<Channel> channel = sptr->getChannel(chanNr);

        if (!channel) {
            channel = std::shared_ptr<Channel> (new Channel(chanNr, sptr->getSliceId(), sptr->getFrameId()));
            sptr->addChannel(channel);
        }

        err = H5Giterate(group_id, name, NULL, process_objects_frames_slices_channels_objects, &(*channel));
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
        int frameNr = atoi (name);
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
    Project *project = static_cast<Project*> (opdata);

    if (statbuf.type == H5G_GROUP) {
        Group objGroup(H5Gopen(group_id, name, H5P_DEFAULT));

        uint32_t objId = readSingleValue<uint32_t>(objGroup, "id");
        uint32_t frameId = readSingleValue<uint32_t>(objGroup, "frame_id");
        uint32_t chanId = readSingleValue<uint32_t>(objGroup, "channel_id");
        uint32_t sliceId = readSingleValue<uint32_t>(objGroup, "slice_id");
        int autoId = readSingleValue<int>(H5Dopen(group_id, "track_id", H5P_DEFAULT));

        std::shared_ptr<AutoTracklet> autoTracklet = project->getAutoTracklet(autoId);
        std::shared_ptr<Frame> frame = project->getMovie()->getFrame(frameId);
        std::shared_ptr<Object> object = frame->getSlice(sliceId)->getChannel(chanId)->getObject(objId);

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
herr_t ImportHDF5::process_autotracklets_daughters(hid_t group_id_o, const char *name, void *opdata) {
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
herr_t ImportHDF5::process_autotracklets (hid_t group_id, const char *name, void *op_data) {
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

        err = H5Giterate(group_id, name, NULL, process_autotracklets_objects, &(*project));

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
        MessageRelay::emitUpdateDetailMax(getGroupSize(file.getId(),"tracks"));
        err = H5Giterate(file.getId(), "tracks", NULL, process_autotracklets, &(*proj));
    } catch (H5::GroupIException &e) {
        throw CTFormatException ("Format mismatch while trying to read autotracklets: " + e.getDetailMsg());
    }

    return !err;
}

bool ImportHDF5::loadTracklets(H5File file, std::shared_ptr<Project> proj)
{
    herr_t err = 0;

    try {
    } catch (H5::GroupIException &e) {
        throw CTFormatException ("Format mismatch while trying to read tracklets: " + e.getDetailMsg());
    }

    return !err;
}

bool ImportHDF5::loadDaughterRelations(H5File file, std::shared_ptr<Project> proj) {
    herr_t err = 0;

    try {
        MessageRelay::emitUpdateDetailMax(getGroupSize(file.getId(),"tracks"));
        err = H5Giterate(file.getId(), "tracks", NULL, process_autotracklets_daughters, &(*proj));
    } catch (H5::GroupIException &e) {
        throw CTFormatException ("Format mismatch while trying to read mother-daughter relations: " + e.getDetailMsg());
    }

    return !err;
}

}
