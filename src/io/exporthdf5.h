#ifndef EXPORTHDF5_H
#define EXPORTHDF5_H

#include "export.h"

#include <memory>

#include <QString>
#include <H5Cpp.h>

#include "hdf5_aux.h"
#include "project.h"

namespace CellTracker {

class ExportHDF5 : public Export
{
public:
    ExportHDF5();
    bool save(std::shared_ptr<Project>, QString);
private:
    bool saveTracklets(H5::H5File, std::shared_ptr<Project>);
    bool saveAnnotations(H5::H5File, std::shared_ptr<Project>);
};

}

#endif // EXPORTHDF5_H
