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
#ifndef IDPROVIDER_H
#define IDPROVIDER_H

#include <cstdint>
#include <set>

namespace CellTracker {

/*!
 * \brief The IdProvider class
 *
 * The IdProvider is responsible for providing Tracklets and Annotations with
 * unique IDs.
 *
 * One can request a new ID using the getNew*Id()-methods. Once the ID is no longer
 * needed, it should be returned using the return*Id()-method.
 */
class IdProvider
{
public:
    static uint32_t getNewTrackletId();
    static void returnTrackletId(uint32_t id);

    static uint32_t getNewAnnotationId();
    static bool claimAnnotationId(uint32_t id);
    static void returnAnnotationId(uint32_t id);

    static uint32_t getNewAutoTrackletId();
    static bool claimAutoTrackletId(uint32_t id);
    static void returnAutoTrackletId(uint32_t id);

private:
    static std::set<uint32_t> usedTrackletIds;
    static uint32_t minFreeTrackletId;
    static std::set<uint32_t> usedAnnotationIds;
    static uint32_t minFreeAnnotationId;
    static std::set<uint32_t> usedAutoTrackletIds;
    static uint32_t minFreeAutoTrackletId;
};

}

#endif // IDPROVIDER_H
