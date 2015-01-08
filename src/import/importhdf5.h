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
    bool loadImages(std::shared_ptr<H5::H5File>, std::shared_ptr<Project>);
    bool loadObjects(std::shared_ptr<H5::H5File>, std::shared_ptr<Project>);
    bool loadAutoTracklets(std::shared_ptr<H5::H5File>, std::shared_ptr<Project>);
    bool loadExportedTracks(std::shared_ptr<H5::H5File> file, std::shared_ptr<Project> project);
    std::shared_ptr<Project> setupEmptyProject();

};

#endif // IMPORTHDF5_H
