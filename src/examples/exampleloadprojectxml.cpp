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
#include <sstream>

#include "io/importxml.h"

/*!
 * \brief demonstrates loading a project from XML
 * \warning most likely broken, as XML is no more supported
 */
void exampleLoadProjectXML() {
    /* Load a movie */
    std::shared_ptr<TraCurate::Project> proj;
    TraCurate::ImportXML ix;
    proj = ix.load(EXAMPLE_XML_FILE);

    /* Write the movie to a file */
    QFile file("/home/sebastian/celltest");
    std::ostringstream stream;
    stream << *proj;
    file.open(QIODevice::Truncate|QIODevice::Text|QIODevice::WriteOnly);
    file.write(stream.str().c_str());
    file.flush();
    file.close();
}
