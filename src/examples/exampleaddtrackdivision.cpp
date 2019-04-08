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

#include "io/importhdf5.h"

/*!
 * \brief demonstrates how to add a TrackEventDivision to a Tracklet
 */
void exampleAddTrackDivision() {
    std::shared_ptr<TraCurate::Project> proj;
    TraCurate::ImportHDF5 ih;
    proj = ih.load(CURRENT_EXAMPLE);

    std::shared_ptr<TraCurate::Genealogy> gen = proj->getGenealogy();

    auto m = std::make_shared<TraCurate::Tracklet>();
    auto d1 = std::make_shared<TraCurate::Tracklet>();
    auto d2 = std::make_shared<TraCurate::Tracklet>();
    auto d3 = std::make_shared<TraCurate::Tracklet>();

    int fId = 0;
    int mId = 3343, d1Id = 3344, d2Id = 3345, d3Id = 3346;
    int o1Id = 4711, o2Id = 4712, o3Id = 4713;

    auto f = std::make_shared<TraCurate::Frame>(fId);
    auto chan = std::make_shared<TraCurate::Channel>(0, 0, 0);
    auto o1 = std::make_shared<TraCurate::Object>(o1Id, chan);
    auto o2 = std::make_shared<TraCurate::Object>(o2Id, chan);
    auto o3 = std::make_shared<TraCurate::Object>(o3Id, chan);

    d1->addToContained(f, o1);
    d1->addToContained(f, o2);
    d1->addToContained(f, o3);

    m->setId(mId);
    d1->setId(d1Id);
    d2->setId(d2Id);
    d3->setId(d3Id);

    gen->addTracklet(m);
    gen->addTracklet(d1);
    gen->addTracklet(d2);
    gen->addTracklet(d3);

    gen->addDaughterTrack(m,o1);
    gen->addDaughterTrack(m,o2);
    gen->addDaughterTrack(m,o3);

    std::cerr << *(gen->getTracklet(mId)) << std::endl;
}
