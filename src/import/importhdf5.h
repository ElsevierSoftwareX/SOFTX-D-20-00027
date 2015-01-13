#ifndef IMPORTHDF5_H
#define IMPORTHDF5_H

#include <H5Cpp.h>
#include "import.h"

class ImportHDF5 : public Import
{
public:
    ImportHDF5();
    std::shared_ptr<Project> load(QString);

private:
    bool loadAnnotations(hid_t, std::shared_ptr<Project>);
    bool loadImages(hid_t, std::shared_ptr<Project>);
    bool loadObjects(hid_t, std::shared_ptr<Project>);
    bool loadAutoTracklets(hid_t, std::shared_ptr<Project>);
    bool loadExportedTracks(hid_t, std::shared_ptr<Project>);
    std::shared_ptr<Project> setupEmptyProject();

};

#endif // IMPORTHDF5_H
