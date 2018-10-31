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
#include "import.h"

#include "project.h"
#include "base/info.h"
#include "base/movie.h"
#include "tracked/genealogy.h"

namespace CellTracker {

Import::~Import() {}

/*!
 * \brief sets up an empty Project and instantiates all required Objects (Info,
 * Movie, Genealogy) to work on it.
 *
 * \return the generated Project
 */
std::shared_ptr<Project> Import::setupEmptyProject()
{
    auto info      = std::make_shared<Info>();
    auto project   = std::make_shared<Project>();
    auto movie     = std::make_shared<Movie>();
    auto genealogy = std::make_shared<Genealogy>(project);

    project->setInfo(info);
    project->setMovie(movie);
    project->setGenealogy(genealogy);

    return project;
}

}
