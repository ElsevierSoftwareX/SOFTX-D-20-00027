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

#include "exceptions/ctexception.h"
#include "io/importhdf5.h"

/*!
 * \brief demonstrates how to add an Annotation
 */
void exampleAddAnnotation() {
    std::shared_ptr<TraCurate::Project> proj;
    TraCurate::ImportHDF5 ih;
    try {
        proj = ih.load(CURRENT_EXAMPLE);

        int nr = 0;
        std::shared_ptr<TraCurate::Genealogy> g = proj->getGenealogy();
        std::shared_ptr<TraCurate::Tracklet> t = g->getTracklet(nr);
        QString title("Some annotation title");
        QString desc("Some annotation description");
        auto a = std::make_shared<TraCurate::Annotation>(TraCurate::Annotation::TRACKLET_ANNOTATION, title, desc);

        proj->getGenealogy()->addAnnotation(a);
        proj->getGenealogy()->annotate(t,a);

        std::shared_ptr<QList<std::shared_ptr<TraCurate::Annotation>>> as = proj->getGenealogy()->getAnnotations();
        for (std::shared_ptr<TraCurate::Annotation> an : *as)
            std::cerr << *an;
    } catch (TraCurate::CTException &e) {
        std::cerr << e.what();
    }

}
