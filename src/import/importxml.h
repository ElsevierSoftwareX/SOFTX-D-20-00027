#ifndef IMPORTXML_H
#define IMPORTXML_H

#include "import.h"

#include <memory>

#include <QDir>
#include <QString>

#include "project.h"

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
};

#endif // IMPORTXML_H
