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
#ifndef IMPORT_H
#define IMPORT_H

#include <memory>

#include <QString>

#include "project.h"

namespace CellTracker {

#define DEFAULT_CHANNEL 0
#define DEFAULT_SLICE 0

/*!
 * \brief The Import class
 *
 * Superclass for Import-Classes that load a Project from a certain resource
 * that can be identified by a QString.
 *
 * Also provides a method to set up an empty Project.
 */
class Import
{
public:
    Import() = default;
    virtual ~Import() = 0;

    virtual std::shared_ptr<Project> load(QString) = 0;
    virtual std::shared_ptr<QImage> requestImage(QString path, int frame, int slice, int channel) = 0;

protected:
    std::shared_ptr<Project> setupEmptyProject();
};

}

#endif // IMPORT_H
