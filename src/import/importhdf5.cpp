#include "importhdf5.h"

#include <QDebug>

ImportHDF5::ImportHDF5()
{
}

std::shared_ptr<Project> ImportHDF5::load(QString fileName)
{
    std::shared_ptr<Project> proj;

    try {
        std::shared_ptr<H5::H5File> file (new H5::H5File(fileName.toStdString().c_str(), H5F_ACC_RDONLY));

        proj = setupEmptyProject();

        qDebug() << "Opened file!";

        loadImages(file, proj);
        loadObjects(file,proj);
        loadAutoTracklets(file,proj);
        loadExportedTracks(file,proj);

        file->close();

        std::cout << *proj;
    } catch (H5::FileIException &e) {
        qDebug() << "Opening the file"
                 << fileName
                 << "failed:"
                 << QString(e.getDetailMsg().c_str());
        return nullptr;
    }


    return proj;
}

std::shared_ptr<Project> ImportHDF5::setupEmptyProject() {
    std::shared_ptr<Project> project(new Project());
    std::shared_ptr<Movie> movie(new Movie());
    std::shared_ptr<Genealogy> genealogy(new Genealogy());

    project->setMovie(movie);
    project->setGenealogy(genealogy);

    return project;
}

bool ImportHDF5::loadImages(std::shared_ptr<H5::H5File> file, std::shared_ptr<Project> project) {
    /*! \todo: Not yet in data format */
    return false;
}

herr_t process_object(hid_t loc_id, const char *name, void *op_data) {
    H5G_stat_t statbuf;
    /*qDebug() << "loc_id:" << loc_id << "\n"
             << "object: " << name << "\n"
             << "type: " << statbuf.type;*/

    if(statbuf.type == H5G_DATASET) {
        qDebug() << "Dataset";
    }
    return 0;
}

herr_t process_frame(hid_t loc_id, const char *name, void *op_data) {
    H5G_stat_t statbuf;
    /*qDebug() << "frame: " << name << "\n"
             << "type: " << statbuf.type << "\n"
             << "loc_id: " << loc_id;*/

    Project *pptr = static_cast<Project *> (op_data);

    if (statbuf.type == H5G_GROUP){
        int frameNr = atoi(name);

        if (!pptr->getMovie()->getFrame(frameNr)) {
            std::shared_ptr<Frame> frame (new Frame(frameNr));
            std::shared_ptr<Slice> slice (new Slice(DEFAULT_SLICE));
            std::shared_ptr<Channel> chan (new Channel(DEFAULT_CHANNEL));

            pptr->getMovie()->addFrame(frame);
            frame->addSlice(slice);
            slice->addChannel(chan);
        }

        H5Giterate(loc_id, name, NULL, process_object, NULL);
    }

    return 0;
}


bool ImportHDF5::loadObjects(std::shared_ptr<H5::H5File> file, std::shared_ptr<Project> project) {
    /* located in HDF5 file at /frames/<frameID>/<objectID>/ */
//    H5::DataSet dataSet = file->openDataSet("conservation-tracking.h5");
    std::string base = "/frames";

    H5::Group group = file->openGroup(base);
    hsize_t frameCount = group.getNumObjs();
    unsigned long long frame;

    hid_t loc_id = group.getId();

    /* C function for iterating, as the one for C++ isn't finished yet */
    H5Giterate(loc_id, base.c_str(), NULL, process_frame, &(*project));
    //H5Literate(loc_id, H5_INDEX_NAME, H5_ITER_NATIVE, nullptr, process_frame, nullptr);

#if 0
    for (frame = 0; frame < frameCount; frame++) {
        /*! \todo: Assumes there is a group for every frame. Check if it exists */
        std::string currFrame = base + "/" + std::to_string(frame);
        qDebug() << "Currently at: " << QString(currFrame.c_str());

        H5::Group frameGroup = file->openGroup(currFrame);
        frameGroup.iterateElems()
    }
#endif


    /*qDebug() << "group.getId(): " << group.getId() << "\n"
             << "group.getObjName(): " << QString(group.getObjName().c_str())
             << "group.getNumObjs(): " << group.getNumObjs();*/

    return false;
}
bool ImportHDF5::loadAutoTracklets(std::shared_ptr<H5::H5File> file, std::shared_ptr<Project> project) { return false; }
bool ImportHDF5::loadExportedTracks(std::shared_ptr<H5::H5File> file, std::shared_ptr<Project> project) { return false; }
