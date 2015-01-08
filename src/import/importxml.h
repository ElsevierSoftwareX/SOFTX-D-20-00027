#ifndef IMPORTXML_H
#define IMPORTXML_H

#include <QDir>
#include "import.h"

class ImportXML : public Import
{
public:
    ImportXML();
    std::shared_ptr<Project> load(QString);

private:
    bool loadImages(const QDir, std::shared_ptr<Project>);
    bool loadObjects(const QDir, std::shared_ptr<Project>);
    bool loadAutoTracklets(const QDir, std::shared_ptr<Project>);
    bool loadExportedTracks(const QDir qd, std::shared_ptr<Project> project);
    std::shared_ptr<Project> setupEmptyProject();
};

#endif // IMPORTXML_H
