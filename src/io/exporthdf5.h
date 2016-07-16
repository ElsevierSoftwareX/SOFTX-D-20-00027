#ifndef EXPORTHDF5_H
#define EXPORTHDF5_H

#include "export.h"

#include <memory>

#include <QString>
#include <H5Cpp.h>

#include "hdf5_aux.h"
#include "tracked/trackevent.h"
#include "project.h"

namespace CellTracker {

/*!
 * \brief The ExportHDF5 class
 *
 * This class provides the means to save a project to the disk using the save()-Method.
 * If the target file does not exist, it will be copied from the old file.
 */
class ExportHDF5 : public Export
{
public:
    ExportHDF5() = default;
    ~ExportHDF5() = default;
    bool save(std::shared_ptr<Project>, QString);
    bool save(std::shared_ptr<Project>, QString, SaveOptions &);
    static bool sanityCheckOptions(std::shared_ptr<Project>, QString, SaveOptions &);

    static bool saveObject(H5::H5File file, std::shared_ptr<Project> proj, std::shared_ptr<Object> obj);

private:
    static bool saveObjects(H5::H5File file, std::shared_ptr<Project> proj);
    static bool saveInfo(H5::H5File file, std::shared_ptr<Project> proj);
    static bool saveImages(H5::H5File file, std::shared_ptr<Project> proj);
    static bool saveAutoTracklets(H5::H5File file, std::shared_ptr<Project> proj);
    static bool saveEvents(H5::H5File file, std::shared_ptr<Project> proj);
    static bool saveTracklets(H5::H5File file, std::shared_ptr<Project> project);
    static bool saveAnnotation(H5::Group grp, std::shared_ptr<Annotation> a);
    static bool saveAnnotations(H5::H5File file, std::shared_ptr<Project> project);
    static bool saveTrackletsContained(H5::H5File file, H5::Group grp, std::shared_ptr<Tracklet> t);
    static bool saveTrackletsNextEvent(H5::Group grp, std::shared_ptr<Tracklet> t);
    static bool saveTrackletsPreviousEvent(H5::Group grp, std::shared_ptr<Tracklet> t);
};

}

#endif // EXPORTHDF5_H
