#ifndef IMPORTXML_H
#define IMPORTXML_H

#include "import.h"

#include <memory>

#include <QDir>
#include <QDomElement>
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
    struct XMLSliceSpec {
        QString tracks;
        QString xml;
        QList<QString> channels;
    };
    struct XMLProjectSpec {
        QString projectFile;
        qint32 rows;
        qint32 cols;
        QList<XMLSliceSpec> slices;
    };

    ImportXML() = default;
    ~ImportXML() = default;

    std::shared_ptr<Project> load(QString);
    std::shared_ptr<Project> load(XMLProjectSpec&);
    std::shared_ptr<QImage> requestImage(QString, int, int, int);
private:
    bool loadFrames(QString, std::shared_ptr<Project> const &, int sliceNr, int channelNr);
    bool loadInfo(QString, std::shared_ptr<Project> const &);
    bool loadObjects(QString, std::shared_ptr<Project> const &, int sliceNr, int channelNr);
    bool loadObjectsInFrame(QString, std::shared_ptr<Channel> &);
    std::shared_ptr<QPolygonF> loadObjectOutline(QDomElement &);
    bool loadAutoTracklets(QString fileName, std::shared_ptr<Project> const &, int sliceNr, int channelNr);

    XMLProjectSpec projectSpec;
};

}

std::ostream &operator<<(std::ostream &os, QString &q);
std::ostream &operator<<(std::ostream &os, CellTracker::ImportXML::XMLSliceSpec &s);
std::ostream &operator<<(std::ostream &os, CellTracker::ImportXML::XMLProjectSpec &p);

#endif // IMPORTXML_H
