#include "exporthdf5.h"

#include <H5Cpp.h>
#include <QDebug>

#include "exceptions/ctexportexception.h"

namespace H5 {

void *HDF5_ERROR_CLIENT_DATA;
H5E_auto2_t HDF5_ERROR_FUNC;

void disableErrors() {
    Exception::getAutoPrint(HDF5_ERROR_FUNC,&HDF5_ERROR_CLIENT_DATA);
    Exception::dontPrint();
}

void enableErrors() {
    Exception::setAutoPrint(HDF5_ERROR_FUNC, HDF5_ERROR_CLIENT_DATA);
}
}

namespace CellTracker {
using namespace H5;

ExportHDF5::ExportHDF5()
{

}

template <typename T>
void writeSingleValue (T value, Group group, const char* name, DataType type) {
    hsize_t dims[] = { 1 };
    DataSpace space(1,dims);
    DataSet set = ExportHDF5::openOrCreateDataSet(group, name, type, space);
    set.write(&value, type);
}

template <typename T>
void writeMultipleValues (T *value, Group group, const char* name, DataType type, int rank, hsize_t *dims) {
    DataSpace space(rank, dims);
    DataSet set = ExportHDF5::openOrCreateDataSet(group, name, type, space);
    set.write(value, type);
}

bool ExportHDF5::groupExists(CommonFG &cfg, const char *name) {
    try {
        H5::disableErrors();
        Group group = cfg.openGroup(name);
        H5::enableErrors();
        /* group exists, return true */
        return true;
    } catch (H5::Exception &e) {
        H5::enableErrors();
        return false;
    }
}

bool ExportHDF5::datasetExists(CommonFG &cfg, const char *name) {
    try {
        H5::disableErrors();
        DataSet ds = cfg.openDataSet(name);
        H5::enableErrors();
        /* group exists, return true */
        return true;
    } catch (H5::Exception &e) {
        H5::enableErrors();
        return false;
    }
}

bool ExportHDF5::linkExists(CommonFG &cfg, const char *name) {
    htri_t ret = H5Lexists(cfg.getLocId(),name,H5P_DEFAULT);

    if(ret >= 0 && ret == true)
        return true;
    else
        return false;
}

/*!
 * \brief opens or, if it does not yet exist, creates a H5::DataSet
 * \param cfg where to place the H5::DataSet (either H5::H5File or H5::Group)
 * \param name the name of the H5::DataSet
 * \param type the H5::DataType of the H5::DataSet
 * \param space the H5::DataSpace of the H5::DataSet
 * \return a H5::DataSet object for the dataset that was opened or created
 */
DataSet ExportHDF5::openOrCreateDataSet(CommonFG& cfg, const char *name, DataType type, DataSpace space) {
    DataSet ds;

    if (datasetExists(cfg, name))
        ds = cfg.openDataSet(name);
    else
        /* DataSet does not exist, create it */
        ds = cfg.createDataSet(name, type, space);

    return ds;
}

/*!
 * \brief opens or, if it does not yet exist, creates a H5::Group
 * \param cfg where to place the H5::Group (either H5::H5File or H5::Group)
 * \param name the name of the H5::Group
 * \param size the expected size of the group
 * \return a H5::Group object for the group that was opened or created
 */
Group ExportHDF5::openOrCreateGroup(CommonFG& cfg, const char *name, int size) {
    Group group;

    if (groupExists(cfg, name))
        group = cfg.openGroup(name);
    else
        /* Group does not exist, create it */
        group = cfg.createGroup(name, size);

    return group;
}

/*!
 * \brief opens or, if it does not yet exist, creates a H5::Group
 * \param cfg where to place the H5::Group (either H5::H5File or H5::Group)
 * \param name the name of the H5::Group
 * \param size the expected size of the group
 * \return a H5::Group object for the group that was opened or created
 */
Group ExportHDF5::clearOrCreateGroup(CommonFG& cfg, const char *name, int size) {
    Group group;

    if (groupExists(cfg, name))
        cfg.unlink(name);

    group = cfg.createGroup(name, size);
    return group;
}

void ExportHDF5::linkOrOverwriteLink(H5L_type_t type, Group grp, std::string target, std::string link_name) {
    if (!linkExists(grp, link_name.c_str())) {
        grp.link(type, target, link_name);
    }
}

bool ExportHDF5::save(std::shared_ptr<Project> project, QString filename)
{
    try {
        H5File file(filename.toStdString().c_str(), H5F_ACC_RDWR, H5P_FILE_CREATE);

        saveTracklets(file, project);
        saveAnnotations(file, project);
    } catch (FileIException &e) {
        throw CTExportException("Saving the HDF5 file failed: " + e.getDetailMsg());
    }

    return true;
}

bool ExportHDF5::saveTracklets(H5File file, std::shared_ptr<Project> project)
{
    std::shared_ptr<QHash<int,std::shared_ptr<Tracklet>>> tracklets = project->getGenealogy()->getTracklets();

    Group trackletsGroup = openOrCreateGroup(file, "/tracklets", tracklets->size());

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

            std::string target = "/objects/frames/"+std::to_string(fId)+"/"+std::to_string(0)+"/"+std::to_string(oId);
            linkOrOverwriteLink(H5L_TYPE_SOFT, trackletGroup, target, std::to_string(fId));
        }

        /* write daughter tracks */
        /*! \todo link or ids? */
    }

    return true;
}

bool ExportHDF5::saveAnnotations(H5File file, std::shared_ptr<Project> project)
{
    std::shared_ptr<QList<std::shared_ptr<Annotation>>> annotations = project->getGenealogy()->getAnnotations();
    Group annotationsGroup = file.openGroup("/annotations");

    QList<std::shared_ptr<Annotation>> objectAnnotations;
    QList<std::shared_ptr<Annotation>> trackAnnotations;

    for (std::shared_ptr<Annotation> a : *annotations) {
        switch (a->getAnnotated()->getAnnotationType()) {
        case Annotateable::OBJECT_ANNOTATION:
            objectAnnotations.push_back(a);
            break;
        case Annotateable::TRACKLET_ANNOTATION:
            trackAnnotations.push_back(a);
            break;
        default:
            throw CTExportException("Unsupported Annotation type");
            break;
        }
    }

    /* object annotations */
    {
        Group oAnno = clearOrCreateGroup(annotationsGroup, "object_annotations", objectAnnotations.size());

        int i = 0;
        for (std::shared_ptr<Annotation> a : objectAnnotations) {
            std::shared_ptr<Object> o = std::static_pointer_cast<Object>(a->getAnnotated());
            StrType st(PredType::C_S1, H5T_VARIABLE);
            Group aGroup = oAnno.createGroup(std::to_string(i), 2);
            writeSingleValue<std::string>(a->getAnnotationText().c_str(), aGroup, "description", st);

            std::string target = "/objects/frames/"
                    + std::to_string(o->getFrameId()) + "/"
                    + std::to_string(o->getSliceId()) + "/"
                    + std::to_string(o->getId())
                    + "/";
            linkOrOverwriteLink(H5L_TYPE_SOFT, aGroup, target, "object");

            i++;
        }

    }

    /* track annotations */
    {
        Group tAnno = clearOrCreateGroup(annotationsGroup, "track_annotations", objectAnnotations.size());

        int i = 0;
        for (std::shared_ptr<Annotation> a : trackAnnotations) {
            std::shared_ptr<Tracklet> t = std::static_pointer_cast<Tracklet>(a->getAnnotated());
            StrType st(PredType::C_S1, H5T_VARIABLE);
            Group aGroup = tAnno.createGroup(std::to_string(i), 2);
            writeSingleValue<std::string>(a->getAnnotationText().c_str(), aGroup, "description", st);

            std::string target = "/tracklets/"
                    + std::to_string(t->getID())
                    + "/";
            linkOrOverwriteLink(H5L_TYPE_SOFT, aGroup, target, "track");

            i++;
        }

    }
    return true;
}

}
