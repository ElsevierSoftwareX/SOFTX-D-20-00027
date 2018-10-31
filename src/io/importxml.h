/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2016, 2015 Sebastian Wagner
 *
 * Celltracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Celltracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Celltracker.  If not, see <https://www.gnu.org/licenses/>.
 */
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
    ImportXML() = default;
    ~ImportXML() = default;

    std::shared_ptr<Project> load(QString);
    std::shared_ptr<Project> load(Project::XMLProjectSpec&);
    std::shared_ptr<QImage> requestImage(QString, int, int, int);
private:
    bool loadFrames(QString, std::shared_ptr<Project> const &, int sliceNr, int channelNr);
    bool loadInfo(QString, std::shared_ptr<Project> const &);
    bool loadObjects(QString, std::shared_ptr<Project> const &, int sliceNr, int channelNr);
    bool loadObjectsInFrame(QString, std::shared_ptr<Channel> &);
    std::shared_ptr<QPolygonF> loadObjectOutline(QDomElement &);
    bool loadAutoTracklets(QString fileName, std::shared_ptr<Project> const &, int sliceNr, int channelNr);
};

}

std::ostream &operator<<(std::ostream &os, QString &q);
std::ostream &operator<<(std::ostream &os, CellTracker::Project::XMLSliceSpec &s);
std::ostream &operator<<(std::ostream &os, CellTracker::Project::XMLProjectSpec &p);

#endif // IMPORTXML_H
