#ifndef IMPORTHDF5_H
#define IMPORTHDF5_H

#include "import.h"

#include <memory>

#include <QString>
#include <QImage>
#include <H5Cpp.h>

#include "project.h"

class ImportHDF5 : public Import
{
public:
    ImportHDF5();
    std::shared_ptr<Project> load(QString);
    std::shared_ptr<QImage> requestImage(QString, int, int, int);

private:
    bool loadInfo(H5::H5File, std::shared_ptr<Project>);
    bool loadAnnotations(H5::H5File, std::shared_ptr<Project>);
    bool loadImages(H5::H5File, std::shared_ptr<Project>);
    bool loadObjects(H5::H5File, std::shared_ptr<Project>);
    bool loadTracklets(H5::H5File, std::shared_ptr<Project>);
};

#endif // IMPORTHDF5_H
