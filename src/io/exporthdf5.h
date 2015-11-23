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

class ExportHDF5 : public Export
{
public:
    ExportHDF5();
    ~ExportHDF5();
    bool save(std::shared_ptr<Project>, QString);
private:
//    bool saveEvents(H5::H5File, std::shared_ptr<TrackEvent<Tracklet>>);
    bool saveEvents(H5::H5File, std::shared_ptr<Project>);
    bool saveTracklets(H5::H5File, std::shared_ptr<Project>);
    bool saveAnnotation(H5::Group, std::shared_ptr<Annotation>);
    bool saveAnnotations(H5::H5File, std::shared_ptr<Project>);
    bool saveAnnotationAssignments(H5::H5File file, H5::Group grp, std::shared_ptr<Annotateable> a);
    bool saveTrackletsContained(H5::H5File file, H5::Group grp, std::shared_ptr<Tracklet> t);
    bool saveTrackletsAnnotations(H5::H5File file, H5::Group grp, std::shared_ptr<Tracklet> t);
    bool saveTrackletsNextEvent(H5::Group grp, std::shared_ptr<Tracklet> t);
    bool saveTrackletsPreviousEvent(H5::Group grp, std::shared_ptr<Tracklet> t);
};

}

#endif // EXPORTHDF5_H
