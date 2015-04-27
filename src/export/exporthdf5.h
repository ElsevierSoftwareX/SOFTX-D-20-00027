#ifndef EXPORTHDF5_H
#define EXPORTHDF5_H

#include "export.h"

#include <memory>

#include <QString>
#include <H5Cpp.h>

#include "project.h"

namespace CellTracker {

class ExportHDF5 : public Export
{
public:
    ExportHDF5();
    bool save(std::shared_ptr<Project>, QString);

    static H5::DataSet openOrCreateDataSet(H5::CommonFG& cfg, const char *name, H5::DataType type, H5::DataSpace space);
    static H5::Group openOrCreateGroup(H5::CommonFG& cfg, const char *name, int size = 0);

    static bool groupExists(H5::CommonFG &cfg, const char *name);
    static bool datasetExists(H5::CommonFG &cfg, const char *name);
    static bool linkExists(H5::CommonFG &cfg, const char *name);

    static void linkOrOverwriteLink(H5L_type_t type, H5::Group grp, std::string target, std::string link_name);
private:
    bool saveTracklets(H5::H5File, std::shared_ptr<Project>);
    bool saveAnnotations(H5::H5File, std::shared_ptr<Project>);
};

}

#endif // EXPORTHDF5_H
