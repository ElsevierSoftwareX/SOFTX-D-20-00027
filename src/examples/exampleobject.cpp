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
#include <iostream>
#include <memory>

#include "examples.h"
#include "base/autotracklet.h"
#include "base/frame.h"
#include "base/object.h"

using namespace TraCurate;

void printObjects(std::vector<std::shared_ptr<Object>> os);
void printFrames(std::vector<std::shared_ptr<Frame>> fs);

void printObjects(std::vector<std::shared_ptr<Object>> os) {
    for (std::shared_ptr<Object> o: os) {
        std::cout << "Object:            " << o->getId() << std::endl
                  << " frameId:          " << o->getFrameId() << std::endl
                  << " isinAutoTracklet: " << (o->isInAutoTracklet()?"true":"false") << std::endl
                  << " autoId:           " << o->getAutoId() << std::endl
                  << " isInTracklet:     " << (o->isInTracklet()?"true":"false") << std::endl
                  << " trackId:          " << o->getTrackId() << std::endl;
    }
}

void printFrames(std::vector<std::shared_ptr<Frame>> fs) {
    for (std::shared_ptr<Frame> f: fs) {
        std::cout << "Frame: " << f->getID() << std::endl;
    }
}

/*!
 * \brief demonstrates writing of Frames and Objects in AutoTracklets
 */
void exampleObject() {
    uint32_t obj1Id, obj2Id, obj3Id, obj4Id,
            frame1Id, frame2Id, frame3Id;

    obj1Id = 0;
    obj2Id = 1;
    obj3Id = 2;
    obj4Id = 3;
    frame1Id = 0;
    frame2Id = 1;
    frame3Id = 2;

    auto f1 = std::make_shared<Frame>(frame1Id);
    auto f2 = std::make_shared<Frame>(frame2Id);
    auto f3 = std::make_shared<Frame>(frame3Id);

    auto s1 = std::make_shared<Slice>(0, f1->getID());
    auto s2 = std::make_shared<Slice>(0, f2->getID());
    auto s3 = std::make_shared<Slice>(0, f3->getID());

    auto chan1 = std::make_shared<Channel>(0, s1->getSliceId(), f1->getID());
    auto chan2 = std::make_shared<Channel>(0, s2->getSliceId(), f2->getID());
    auto chan3 = std::make_shared<Channel>(0, s3->getSliceId(), f3->getID());

    auto o1 = std::make_shared<Object>(obj1Id, chan1);
    auto o2 = std::make_shared<Object>(obj2Id, chan2);
    auto o3 = std::make_shared<Object>(obj3Id, chan2);
    auto o4 = std::make_shared<Object>(obj4Id, chan3);

    std::vector<std::shared_ptr<Object>> os = {o1,o2,o3,o4};
    std::vector<std::shared_ptr<Frame>> fs = {f1,f2,f3};

    printObjects(os);
    printFrames(fs);

    auto a1 = std::make_shared<AutoTracklet>(3343);
    auto a2 = std::make_shared<AutoTracklet>(3344);

    a1->addComponent(f1, o1);
    a1->addComponent(f2, o2);
    a2->addComponent(f2, o3);

    printObjects(os);
    printFrames(fs);
}
