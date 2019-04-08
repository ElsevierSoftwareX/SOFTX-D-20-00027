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
#include <vector>

#include "examples.h"
#include "provider/idprovider.h"
#include "tracked/tracklet.h"

/*!
 * \brief demonstrates the IDProvider and that redistributen of returned IDs works
 */
void exampleIdProvider() {
    std::cout << TraCurate::IdProvider::getNewTrackletId() << std::endl; // 0
    std::cout << TraCurate::IdProvider::getNewTrackletId() << std::endl; // 1
    std::cout << TraCurate::IdProvider::getNewTrackletId() << std::endl; // 2
    std::cout << TraCurate::IdProvider::getNewTrackletId() << std::endl; // 3
    std::cout << TraCurate::IdProvider::getNewTrackletId() << std::endl; // 4

    TraCurate::IdProvider::returnTrackletId(1);
    TraCurate::IdProvider::returnTrackletId(3);

    std::cout << TraCurate::IdProvider::getNewTrackletId() << std::endl; // 1
    std::cout << TraCurate::IdProvider::getNewTrackletId() << std::endl; // 3

    for (uint32_t i = 0; i < 5; i++)
        TraCurate::IdProvider::returnTrackletId(i);

    std::cout << "Testing speed" << std::endl;
    std::cout << "inserting numbers" << std::endl;
    for (uint32_t i = 0; i < 100000; i++)
        TraCurate::IdProvider::getNewTrackletId();

    std::cout << "deleting numbers" << std::endl;
    for (uint32_t i = 100000; i > 0; i--)
        TraCurate::IdProvider::returnTrackletId(i-1);

    std::cout << TraCurate::IdProvider::getNewTrackletId() << std::endl; // 1

    auto t1 = std::make_shared<TraCurate::Tracklet>();
    auto t2 = std::make_shared<TraCurate::Tracklet>();
    auto t3 = std::make_shared<TraCurate::Tracklet>();
    auto t4 = std::make_shared<TraCurate::Tracklet>();
    auto t5 = std::make_shared<TraCurate::Tracklet>();

    std::vector<std::shared_ptr<TraCurate::Tracklet>> ts = {t1,t2,t3,t4,t5};
    for (std::shared_ptr<TraCurate::Tracklet> t: ts)
        std::cout << t->getId() << std::endl;
    ts.clear();

    t2.reset();
    t4.reset();

    auto t6 = std::make_shared<TraCurate::Tracklet>();
    auto t7 = std::make_shared<TraCurate::Tracklet>();

    ts = {t1,t3,t5,t6,t7};
    for (std::shared_ptr<TraCurate::Tracklet> t: ts)
        std::cout << t->getId() << std::endl;
}
