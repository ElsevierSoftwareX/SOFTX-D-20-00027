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
#include "examples.h"

#include <memory>

#include "exceptions/tcexception.h"
#include "io/importhdf5.h"

/*!
 * \brief demonstrates how to load a project from HDF5
 */
void exampleLoadProjectHDF5 () {
    std::shared_ptr<TraCurate::Project> proj;
    TraCurate::ImportHDF5 ih;
    try {
        proj = ih.load(CURRENT_EXAMPLE);

        std::cerr << *proj;
    } catch (TraCurate::TCException &e) {
        std::cerr << e.what();
    }
}
