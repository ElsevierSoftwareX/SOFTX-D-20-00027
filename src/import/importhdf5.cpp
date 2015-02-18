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

using namespace H5;

ImportHDF5::ImportHDF5()
{
}

std::shared_ptr<Project> ImportHDF5::load(QString fileName)
{
    std::shared_ptr<Project> proj;

    try {
        H5File file (fileName.toStdString().c_str(),H5F_ACC_RDONLY);

        proj = setupEmptyProject();
        qDebug() << "Not loading info";
//        loadInfo(file,proj);
        qDebug() << "Loading annotations";
        loadAnnotations(file,proj);
        qDebug() << "Not loading images";
//        loadImages(file,proj);
        qDebug() << "Loading objects";
        loadObjects(file,proj);
        qDebug() << "Loading tracklets";
        loadTracklets(file, proj);
        qDebug() << "Finished";

        file.close();
    } catch (H5::FileIException &e) {
        qDebug() << "Opening the file"
                 << fileName
                 << "failed:"
                 << QString(e.getDetailMsg().c_str());
        return nullptr;
    } catch (std::string &e) {
        qDebug() << e.c_str();
        exit(-1);
    }

    return proj;
}

/*!
 * \brief does not close the DataSet after reading
 */
template <class T> inline T readSingleValue(DataSet dset) {
    T ret;
    DataType dtype = dset.getDataType();

    dset.read(&ret, dtype);

    dtype.close();
    return ret;

}

/*!
 * \brief does close the DataSet after reading
 */
template <class T> inline T readSingleValue(hid_t dset_id) {
    return readSingleValue<T>(DataSet(dset_id));
}

/*!
 * \brief does not close the group after reading
 */
template <class T> inline T readSingleValue(Group group, const char *name) {
    T ret;
    DataSet dset = group.openDataSet(name);

    ret = readSingleValue<T>(dset);

    dset.close();
    return ret;
}

/*!
 * \brief does close the group after reading
 */
template <class T> inline T readSingleValue(hid_t group_id, const char *name) {
    return readSingleValue<T>(Group(group_id), name);
}

template <class T> inline std::tuple<T *,hsize_t *,int> readMultipleValues(DataSet dset) {
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

    dspace.close();
    dtype.close();

    return std::make_tuple(buf,dims,rank);
}

template <class T> inline std::tuple<T *,hsize_t *,int> readMultipleValues(hid_t dset_id) {
    return readMultipleValues<T>(DataSet (dset_id));
}

template <class T> inline std::tuple<T *,hsize_t *,int> readMultipleValues(Group group, const char *name) {
    DataSet dset = group.openDataSet(name);

    auto ret = readMultipleValues<T>(dset);

    dset.close();
    return ret;
}

template <class T> inline std::tuple<T *, hsize_t *, int> readMultipleValues(hid_t group_id, const char *name) {
    return readMultipleValues<T>(Group(group_id), name);
}

bool ImportHDF5::loadInfo (H5File file, std::shared_ptr<Project> proj) {
    Group info = file.openGroup("info");
    {
        /*! \todo Haven't found a way to make this work yet */
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

        datatype.close();
        timeOfConversion.close();
    }
    {
        Group trackingInfo = info.openGroup("tracking_info");
        {
            std::string algo;
            DataSet algorithm = trackingInfo.openDataSet("algorithm");
            DataType datatype = algorithm.getDataType();

            algorithm.read(algo,datatype);
            proj->getInfo()->setTrackingInfoAlgorithm(algo);

            datatype.close();
            algorithm.close();
        }
        {
            std::string vers;
            DataSet version = trackingInfo.openDataSet("ilastik_version");
            DataType datatype = version.getDataType();

            version.read(vers, datatype);
            proj->getInfo()->setTrackingInfoILastikVersion(vers);

            datatype.close();
            version.close();
        }
        {
            std::string time;
            DataSet timeOfTracking = trackingInfo.openDataSet("timeOfTracking");
            DataType datatype = timeOfTracking.getDataType();

            timeOfTracking.read(time,datatype);
            QLocale enUS("en_US");
            QDateTime datetime = enUS.toDateTime(time.c_str(), "ddd MMM dd HH:mm:ss yyyy");
            proj->getInfo()->setTrackingInfoTimeOfTracking(datetime);

            datatype.close();
            timeOfTracking.close();
        }
        trackingInfo.close();
    }
    info.close();
    return true;
}

herr_t process_track_annotations (hid_t group_id, const char *name, void *op_data) {
    QList<Annotation> *annotations = static_cast<QList<Annotation> *>(op_data);

    Group annotationElement (H5Gopen(group_id,name,H5P_DEFAULT));
    std::string text = readSingleValue<std::string>(annotationElement,"description");

    Annotation newAnnotation(Annotation::TRACK_ANNOTATION,nullptr);
    newAnnotation.setText(text);
    annotations->append(newAnnotation);

    annotationElement.close();

    return 0;
}

bool ImportHDF5::loadAnnotations(H5File file, std::shared_ptr<Project> proj) {
    Group annotations = file.openGroup("annotations");
    {
        std::shared_ptr<QList<Annotation>> anno = proj->getGenealogy()->getAnnotations();
        if (anno == nullptr) {
            anno = std::shared_ptr<QList<Annotation>>(new QList<Annotation>());
            proj->getGenealogy()->setAnnotations(anno);
        }
        annotations.iterateElems("track_annotations", NULL, process_track_annotations, &(*anno));
    }
    annotations.close();
    return true;
}

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

    sliceGroup.close();
    frameGroup.close();
    framesGroup.close();
    imagesGroup.close();
    file.close();

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

herr_t process_images_frames_slices_channels(hid_t group_id, const char *name, void *op_data) {
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

            // TODO
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
//            delete[] (maxdims);

            channel->setImage(img);
        }
    }
    return 0;
}

herr_t process_images_frames_slices(hid_t group_id, const char *name, void *op_data) {
    herr_t err = 0;
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    Frame* frame = static_cast<Frame*>(op_data);

    if (statbuf.type == H5G_GROUP) {
        int slicenr = atoi(name);

        std::shared_ptr<Slice> slice = frame->getSlice(slicenr);
        if (slice == nullptr) {
            slice = std::shared_ptr<Slice>(new Slice(slicenr));
            frame->addSlice(slice);
        }

        err = H5Giterate(group_id,name,NULL,process_images_frames_slices_channels,&(*slice));
    }

    return err;
}

herr_t process_images_frames(hid_t group_id, const char *name, void *op_data) {
    herr_t err = 0;
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    Movie *movie = static_cast<Movie*>(op_data);

    if (statbuf.type == H5G_GROUP){
        Group frameGroup (H5Gopen(group_id, name, H5P_DEFAULT));
        int framenr = readSingleValue<int>(frameGroup, "id");
        frameGroup.close();

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

bool ImportHDF5::loadImages(H5File file, std::shared_ptr<Project> proj) {
    herr_t err = 0;
    Group images = file.openGroup("images");
    {
        std::shared_ptr<Movie> movie = proj->getMovie();
        err = H5Giterate(images.getId(),"frames", NULL, process_images_frames,&(*movie));
    }
    images.close();
    return !err;
}

std::shared_ptr<QPoint> readCentroid(hid_t objGroup) {
    std::shared_ptr<QPoint> point(new QPoint());

    auto data = readMultipleValues<uint16_t>(H5Dopen(objGroup,"centroid",H5P_DEFAULT));
    uint16_t *buf = std::get<0>(data);

    point->setX(buf[1]);
    point->setY(buf[0]);

    delete[] (std::get<1>(data));
    delete[] (buf);

    return point;
}

std::shared_ptr<QRect> readBoundingBox(hid_t objGroup) {
    std::shared_ptr<QRect> box (new QRect());

    auto data = readMultipleValues<uint16_t>(H5Dopen(objGroup, "bounding_box", H5P_DEFAULT));
    uint16_t *buf = std::get<0>(data);

    box->setCoords(buf[0], buf[1], buf[2], buf[3]);

    delete[] (std::get<1>(data));
    delete[] (buf);

    return box;
}

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

herr_t process_frames_slices_objects_properties(hid_t group_id, const char *name, void *op_data) {
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
        } else if (!sname.compare("packed_mask")) {
            /*! \todo unknown, if needed. if it is, the code is at the end of this file */
        } /* frame_id, slice_id and id are already given via the path */
    }

    return 0;
}

herr_t process_objects_frames_slices_objects (hid_t group_id, const char *name, void *op_data) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    herr_t err = 0;
    Slice *sptr = static_cast<Slice *> (op_data);

    if (statbuf.type == H5G_GROUP) {
        Group objGroup (H5Gopen(group_id, name, H5P_DEFAULT));
        uint32_t objNr = readSingleValue<uint32_t>(objGroup,"id");

        std::shared_ptr<Object> object = sptr->getObject(objNr);

        if (!object) {
            object = std::shared_ptr<Object> (new Object(objNr));
            sptr->addObject(object);
        }

        err = H5Giterate(group_id, name, NULL, process_frames_slices_objects_properties, &(*object));
    }

    return err;
}

herr_t process_objects_frames_slices (hid_t group_id, const char *name, void *op_data) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    herr_t err = 0;
    Frame *fptr = static_cast<Frame *> (op_data);

    if (statbuf.type == H5G_GROUP) {
        int sliceNr = atoi(name);
        std::shared_ptr<Slice>  slice = fptr->getSlice(sliceNr);

        if (!slice) {
            slice = std::shared_ptr<Slice> (new Slice(sliceNr));
            fptr->addSlice(slice);
        }

        err = H5Giterate(group_id, name, NULL, process_objects_frames_slices_objects, &(*slice));
    }

    return err;
}

herr_t process_objects_frames(hid_t group_id, const char *name, void *op_data) {
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

    return err;
}

bool ImportHDF5::loadObjects(H5File file, std::shared_ptr<Project> proj) {
    herr_t err = 0;
    Group objects = file.openGroup("objects");
    {
        std::shared_ptr<Movie> movie = proj->getMovie();
        err = H5Giterate(objects.getId(), "frames", NULL, process_objects_frames, &(*movie));
    }
    objects.close();
    return !err;
}

herr_t process_tracklets_objects(hid_t group_id, const char *name, void *opdata) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    Project *project = static_cast<Project*> (opdata);

    if (statbuf.type == H5G_GROUP) {
        Group objGroup(H5Gopen(group_id, name, H5P_DEFAULT));

        uint32_t objId = readSingleValue<uint32_t>(objGroup, "id");
        uint32_t frameId = readSingleValue<uint32_t>(objGroup, "frame_id");
        uint32_t sliceId = readSingleValue<uint32_t>(objGroup, "slice_id");
        int trackId = readSingleValue<int>(H5Dopen(group_id, "track_id", H5P_DEFAULT));

        std::shared_ptr<Tracklet> tracklet = project->getGenealogy()->getTracklet(trackId);
        std::shared_ptr<Frame> frame = project->getMovie()->getFrame(frameId);
        std::shared_ptr<Object> object = frame->getSlice(sliceId)->getObject(objId);

        if (frame == nullptr)
            qDebug() << "Did not find frame" << frameId << "in Movie";
        if (tracklet == nullptr)
            qDebug() << "Did not find tracklet" << trackId << "in genealogy";
        if (object == nullptr)
            qDebug() << "Did not find object" << objId << "in slice" << sliceId << "of frame" << frameId;

        if (tracklet != nullptr && object != nullptr && frame != nullptr) {
            tracklet->addToContained(frame,object);
        } else {
            qDebug() << "Error while adding object" << objId << "at frame" << frameId << "to track" << trackId;
        }

        /*! \todo mother/daugthers */
    }

    return 0;
}

herr_t process_tracklets_daughters(hid_t group_id_o, const char *name, void *opdata) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id_o, name, true, &statbuf);
    Project *project = static_cast<Project*> (opdata);
    hid_t group_id = H5Gopen(group_id_o, name, H5P_DEFAULT);

    if (statbuf.type == H5G_GROUP) {
        int trackId = readSingleValue<int>(H5Dopen(group_id, "track_id", H5P_DEFAULT));
        QList<std::shared_ptr<Tracklet>> daughters;

        {
            // Get daughters
            htri_t ret = H5Lexists(group_id, "daughters", H5P_DEFAULT);
            if (ret >= 0 && ret == true) {
                auto data = readMultipleValues<uint32_t>(H5Dopen(group_id, "daughters", H5P_DEFAULT));
                uint32_t *buf = std::get<0>(data);
                hsize_t *dims = std::get<1>(data);
                int rank = std::get<2>(data);

                if (rank == 1){
                    for (hsize_t i = 0; i < dims[0]; i++) {
                        int idx = static_cast<int>(buf[i]);
                        std::shared_ptr<Tracklet> daughter = project->getGenealogy()->getTracklet(idx);
                        if (daughter) {
                            daughters.append(daughter);
                        }
                    }
                }
            }
        }

        std::shared_ptr<Tracklet> tracklet = project->getGenealogy()->getTracklet(trackId);

        if (tracklet == nullptr)
            qDebug() << "Did not find tracklet" << trackId << "in genealogy";
        if (!daughters.isEmpty() && tracklet != nullptr) {
            std::shared_ptr<TrackEventDivision> event = std::shared_ptr<TrackEventDivision>(new TrackEventDivision());
            event->setNext(daughters);
            tracklet->setNext(event);
        }

        /*! \todo mother/daugthers */
    }

    return 0;
}

herr_t process_tracklets (hid_t group_id, const char *name, void *op_data) {
    H5G_stat_t statbuf;
    H5Gget_objinfo(group_id, name, true, &statbuf);
    herr_t err = 0;
    Project *project = static_cast<Project*>(op_data);

    if (statbuf.type == H5G_GROUP) {
        Group trackGroup (H5Gopen(group_id, name, H5P_DEFAULT));
        int tracknr = readSingleValue<int>(trackGroup, "track_id");

        std::shared_ptr<Tracklet> tracklet = project->getGenealogy()->getTracklet(tracknr);

        if (!tracklet) {
            tracklet = std::shared_ptr<Tracklet>(new Tracklet());
            tracklet->setID(tracknr);
            project->getGenealogy()->addTracklet(tracklet);
        }

        err = H5Giterate(group_id, name, NULL, process_tracklets_objects, &(*project));

    }

    return err;
}

bool ImportHDF5::loadTracklets(H5File file, std::shared_ptr<Project> project) {
    herr_t err = 0;

    qDebug() << "  Loading tracklets without mother-daughter relation";
    err = H5Giterate(file.getId(), "tracks", NULL, process_tracklets, &(*project));

    /* we don't need to look at mother tracks as this relation is just the reverse of the daughter relation */
    if (!err) {
        qDebug() << "  Setting mother-daughter relation";
        err = H5Giterate(file.getId(), "tracks", NULL, process_tracklets_daughters, &(*project));
    }

    return !err;
}
