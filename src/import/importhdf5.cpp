#include "importhdf5.h"

#include <QDebug>
#include <QImage>
#include <QColor>


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
        loadInfo(file,proj);
        loadAnnotations(file,proj); // loadAnnotations
        loadImages(file,proj);      // loadImages
        loadObjects(file,proj);     // loadObjects
        loadTracklets(file, proj);  // loadAutoTracklets(file,proj);

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
    DataSet description = annotationElement.openDataSet("description");
    DataType dtype = description.getDataType();

    std::string text;
    description.read(text,dtype);

    Annotation newAnnotation(Annotation::TRACK_ANNOTATION,nullptr);
    newAnnotation.setText(text);

    annotations->append(newAnnotation);

    dtype.close();
    description.close();
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
            uint8_t r = buf[posy * offy + posx * offx + 0];
            uint8_t g = buf[posy * offy + posx * offx + 1];
            uint8_t b = buf[posy * offy + posx * offx + 2];
            QColor col(r,g,b);
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
    DataSet imageData = sliceGroup.openDataSet(std::to_string(channel).c_str());

    DataSpace dspace = imageData.getSpace();
    DataType dtype = imageData.getDataType();
    int rank = dspace.getSimpleExtentNdims();
    hsize_t *dims = new hsize_t[rank];
    dspace.getSimpleExtentDims(dims);

    int size = 1;
    for (int i=0; i<rank; i++)
        size *= dims[i];

    uint8_t *buf = new uint8_t[size];
    imageData.read(buf, dtype);

    dtype.close();
    dspace.close();
    imageData.close();
    sliceGroup.close();
    frameGroup.close();
    framesGroup.close();
    imagesGroup.close();
    file.close();

    std::shared_ptr<QImage> img = bufToImage(buf, dims[0], dims[1], dims[2]);

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

            hid_t dchan = H5Dopen(group_id,name,H5P_DEFAULT);

            hid_t dataspace;/*, memspace;*/
            int rank = 3;
            hsize_t *dims = new hsize_t[rank];
            hsize_t *maxdims = new hsize_t[rank];

            dataspace = H5Dget_space(dchan);
            H5Sget_simple_extent_dims(dataspace, dims, maxdims);

            int size = 1;
            for (int i=0; i<rank; ++i)
                size *= maxdims[i];

            uint8_t *buf = new uint8_t[size];

            H5Dread(dchan, H5T_NATIVE_UINT8, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
            H5Dclose(dchan);

            std::shared_ptr<QImage> img = bufToImage(buf, maxdims[0], maxdims[1], maxdims[2]);

            delete[] (buf);
            delete[] (dims);
            delete[] (maxdims);

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
        int framenr = atoi(name);

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
    hid_t centroid = H5Dopen (objGroup, "centroid", H5P_DEFAULT);

    hid_t dataspace, memspace;
    int rank = 1;
    const hsize_t dims[] = {2};
    const hsize_t maxdims[] = {2};
    uint16_t buf[2] = {0,0};

    dataspace = H5Dget_space(centroid);
    memspace = H5Screate_simple(rank, dims, maxdims);
    H5Dread(centroid, H5T_NATIVE_UINT16, memspace, dataspace, H5P_DEFAULT, buf);

    H5Sclose(memspace);
    H5Dclose(centroid);

    point->setX(buf[1]);
    point->setY(buf[0]);

    return point;
}

std::shared_ptr<QRect> readBoundingBox(hid_t objGroup) {
    std::shared_ptr<QRect> box (new QRect());
    hid_t bbox = H5Dopen (objGroup, "bounding_box", H5P_DEFAULT);

    hid_t dataspace, memspace;
    int rank = 2;
    const hsize_t dims[] = {2,2};
    const hsize_t maxdims[] = {2,2};
    uint16_t buf[2][2]  = {{0,0},{0,0}};

    dataspace = H5Dget_space(bbox);
    memspace = H5Screate_simple(rank, dims, maxdims);
    H5Dread(bbox, H5T_NATIVE_UINT16, memspace, dataspace, H5P_DEFAULT, buf);

    H5Sclose(memspace);
    H5Dclose(bbox);

    /*! \todo Check if this is correct */
    box->setCoords(buf[0][0], buf[0][1], buf[1][0], buf[1][1]);

    return box;
}

std::shared_ptr<QPolygonF> readOutline (hid_t objGroup) {
    std::shared_ptr<QPolygonF> poly (new QPolygonF());
    hid_t outline = H5Dopen (objGroup, "outline", H5P_DEFAULT);

    hid_t dataspace, memspace;
    int rank = 2;
    hsize_t *dims = new hsize_t[rank];
    hsize_t *maxdims = new hsize_t[rank];

    dataspace = H5Dget_space(outline);
    H5Sget_simple_extent_dims(dataspace, dims, maxdims);

    hsize_t length = maxdims[0];

    uint32_t *buf = new uint32_t[length * 2];

    memspace = H5Screate_simple(rank, dims, maxdims);
    H5Dread(outline, H5T_NATIVE_UINT32, memspace, dataspace, H5P_DEFAULT, buf);
    H5Dclose(outline);

    for (hsize_t i = 0; i < length; i++) {
        poly->append(QPoint(buf[i*2],buf[i*2 + 1]));
    }
    /* Close the polygon */
    poly->append(QPoint(buf[0],buf[1]));

    delete[] (buf);
    delete[] (dims);
    delete[] (maxdims);

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
        /*! \todo: wrong, get id from id-Dataset */
        uint32_t objNr;
        {
            std::string path  = std::string(name) + "/id";
            hid_t idDS = H5Dopen(group_id, path.c_str(), H5P_DEFAULT);
            H5Dread(idDS, H5T_NATIVE_UINT32, H5S_ALL, H5S_ALL, H5P_DEFAULT, &objNr);
            H5Dclose(idDS);
        }
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
        uint32_t objId;
        uint32_t frameId;
        uint32_t sliceId;
        int trackId;

        {
            // Get frameID => frame_id
            DataSet objIdDataset = objGroup.openDataSet("id");
            DataType dtype = objIdDataset.getDataType();
            objIdDataset.read(&objId, dtype);
            dtype.close();
            objIdDataset.close();
        }

        {
            // Get frameID => frame_id
            DataSet frameIdDataset = objGroup.openDataSet("frame_id");
            DataType dtype = frameIdDataset.getDataType();
            frameIdDataset.read(&frameId, dtype);
            dtype.close();
            frameIdDataset.close();
        }

        {
            // Get objectId => id
            DataSet trackIdDataset (H5Dopen(group_id, "track_id", H5P_DEFAULT));
            DataType dtype = trackIdDataset.getDataType();
            trackIdDataset.read(&trackId, dtype);
            dtype.close();
            trackIdDataset.close();
        }

        {
            // Get sliceId => slice_id
            DataSet sliceIdDataset = objGroup.openDataSet("slice_id");
            DataType dtype = sliceIdDataset.getDataType();
            sliceIdDataset.read(&sliceId, dtype);
            dtype.close();
            sliceIdDataset.close();
        }

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
            qDebug() << "Adding object" << objId << "at frame" << frameId << "to track" << trackId;
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
        int tracknr = atoi(name);
        std::shared_ptr<Tracklet> tracklet = project->getGenealogy()->getTracklet(tracknr);

        if (!tracklet) {
            tracklet = std::shared_ptr<Tracklet>(new Tracklet());
            project->getGenealogy()->addTracklet(tracklet);
        }

        err = H5Giterate(group_id, name, NULL, process_tracklets_objects, &(*project));
    }

    return err;
}

bool ImportHDF5::loadTracklets(H5File file, std::shared_ptr<Project> project) {
    herr_t err = 0;

    err = H5Giterate(file.getId(), "tracks", NULL, process_tracklets, &(*project));

    return !err;
}
