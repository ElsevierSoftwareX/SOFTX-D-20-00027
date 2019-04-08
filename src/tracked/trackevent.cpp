/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2017, 2016, 2015 Sebastian Wagner
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
#include "base/autotracklet.h"
#include "trackevent.h"
#include "trackeventdead.hpp"
#include "trackeventdivision.hpp"
#include "trackeventlost.hpp"
#include "trackeventmerge.hpp"
#include "trackeventunmerge.hpp"

#include <QDebug>

template <typename T>
std::ostream &
operator<<(std::ostream &strm, TraCurate::TrackEvent<T> &t)
{
    strm << "TrackEvent: ";
    switch (t.type) {
    case TraCurate::TrackEvent<T>::EVENT_TYPE_DEAD: {
        strm << "[EVENT_TYPE_DEAD]" << std::endl;
        TraCurate::TrackEventDead<T> &ted = static_cast<TraCurate::TrackEventDead<T>&>(t);
        strm << "prev: " << ted.prev << std::endl; }
        break;
    case TraCurate::TrackEvent<T>::EVENT_TYPE_DIVISION: {
        strm << "[EVENT_TYPE_DIVISION]" << std::endl;
        TraCurate::TrackEventDivision<T> &ted = static_cast<TraCurate::TrackEventDivision<T>&>(t);
        strm << "prev: " << ted.prev << std::endl;
        strm << "next: ";
        for (std::shared_ptr<TraCurate::Tracklet> n: *ted.next)
            strm << n->getId() << " ";
        strm << std::endl; }
        break;
    case TraCurate::TrackEvent<T>::EVENT_TYPE_LOST: {
        strm << "[EVENT_TYPE_LOST]" << std::endl;
        TraCurate::TrackEventLost<T> &tel = static_cast<TraCurate::TrackEventLost<T>&>(t);
        strm << "prev: " << tel.prev << std::endl; }
        break;
    case TraCurate::TrackEvent<T>::EVENT_TYPE_MERGE: {
        strm << "[EVENT_TYPE_MERGE]" << std::endl;
        TraCurate::TrackEventMerge<T> &tem = static_cast<TraCurate::TrackEventMerge<T>&>(t);
        strm << "prev: ";
        for (std::shared_ptr<TraCurate::Tracklet> p: *tem.prev)
            strm << p->getId() << " ";
        strm << std::endl;
        strm << "next: " << tem.next << std::endl; }
        break;
    case TraCurate::TrackEvent<T>::EVENT_TYPE_UNMERGE: {
        strm << "[EVENT_TYPE_UNMERGE]" << std::endl;
        TraCurate::TrackEventUnmerge<T> &teu = static_cast<TraCurate::TrackEventUnmerge<T>&>(t);
        strm << "prev: " << teu.prev << std::endl;
        strm << "next: ";
        for (std::shared_ptr<TraCurate::Tracklet> n: *teu.next)
            strm << n->getId() << " ";
        strm << std::endl; }
        break;
    default:
        break;
    }
    return strm;
}
