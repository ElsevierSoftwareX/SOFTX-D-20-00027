/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2016, 2015 Sebastian Wagner
 *
 * TraCurate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TraCurate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with TraCurate.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef EXPORT_H
#define EXPORT_H

#include <memory>

#include "project.h"

namespace TraCurate {

/*!
 * \brief The Export class
 *
 * Superclass for Classes that save a given Project to a resource that can be
 * identified via a QString.
 *
 * Also provides methods to save to same file that the Project was loaded from.
 */
class Export
{
public:
    struct SaveOptions {
        bool annotations;
        bool autoTracklets;
        bool events;
        bool images;
        bool info;
        bool objects;
        bool tracklets;
    };

    Export() = default;
    virtual ~Export() = 0;

    virtual bool save(std::shared_ptr<Project>,QString) = 0;
    bool save(std::shared_ptr<Project>);
};

}

#endif // EXPORT_H
