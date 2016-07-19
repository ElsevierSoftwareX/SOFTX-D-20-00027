#ifndef IMPORTXML_H
#define IMPORTXML_H

#include "import.h"

#include <memory>

#include <QDir>
#include <QImage>
#include <QString>

#include "project.h"

namespace CellTracker {
/*!
 * \warning DO NOT USE THIS.
 *
 * \brief The ImportXML class
 *
 * This class was used to load projects in the old XML-Format which has been
 * replaced with the new HDF5-Format. Due to massive changes to the backing
 * data structure, the methods in this class most likely won't work anymore.
 */
class ImportXML : public Import
{
public:
    ImportXML() = default;
    ~ImportXML() = default;

    std::shared_ptr<Project> load(QString);
    std::shared_ptr<QImage> requestImage(QString, int, int, int);
private:
    bool loadFrames(QString, std::shared_ptr<Project> const &);

#if 0
    bool loadImages(const QDir, std::shared_ptr<Project>);
    bool loadObjects(const QDir, std::shared_ptr<Project>);
    bool loadAutoTracklets(const QDir, std::shared_ptr<Project>);
    bool loadExportedTracks(const QDir qd, std::shared_ptr<Project> project);
#endif
};

}

#endif // IMPORTXML_H
