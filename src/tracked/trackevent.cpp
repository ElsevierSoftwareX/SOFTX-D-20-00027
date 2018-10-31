/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2017, 2016, 2015 Sebastian Wagner
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
operator<<(std::ostream &strm, CellTracker::TrackEvent<T> &t)
{
    strm << "TrackEvent: ";
    switch (t.type) {
    case CellTracker::TrackEvent<T>::EVENT_TYPE_DEAD: {
        strm << "[EVENT_TYPE_DEAD]" << std::endl;
        CellTracker::TrackEventDead<T> &ted = static_cast<CellTracker::TrackEventDead<T>&>(t);
        strm << "prev: " << ted.prev << std::endl; }
        break;
    case CellTracker::TrackEvent<T>::EVENT_TYPE_DIVISION: {
        strm << "[EVENT_TYPE_DIVISION]" << std::endl;
        CellTracker::TrackEventDivision<T> &ted = static_cast<CellTracker::TrackEventDivision<T>&>(t);
        strm << "prev: " << ted.prev << std::endl;
        strm << "next: ";
        for (std::shared_ptr<CellTracker::Tracklet> n: *ted.next)
            strm << n->getId() << " ";
        strm << std::endl; }
        break;
    case CellTracker::TrackEvent<T>::EVENT_TYPE_LOST: {
        strm << "[EVENT_TYPE_LOST]" << std::endl;
        CellTracker::TrackEventLost<T> &tel = static_cast<CellTracker::TrackEventLost<T>&>(t);
        strm << "prev: " << tel.prev << std::endl; }
        break;
    case CellTracker::TrackEvent<T>::EVENT_TYPE_MERGE: {
        strm << "[EVENT_TYPE_MERGE]" << std::endl;
        CellTracker::TrackEventMerge<T> &tem = static_cast<CellTracker::TrackEventMerge<T>&>(t);
        strm << "prev: ";
        for (std::shared_ptr<CellTracker::Tracklet> p: *tem.prev)
            strm << p->getId() << " ";
        strm << std::endl;
        strm << "next: " << tem.next << std::endl; }
        break;
    case CellTracker::TrackEvent<T>::EVENT_TYPE_UNMERGE: {
        strm << "[EVENT_TYPE_UNMERGE]" << std::endl;
        CellTracker::TrackEventUnmerge<T> &teu = static_cast<CellTracker::TrackEventUnmerge<T>&>(t);
        strm << "prev: " << teu.prev << std::endl;
        strm << "next: ";
        for (std::shared_ptr<CellTracker::Tracklet> n: *teu.next)
            strm << n->getId() << " ";
        strm << std::endl; }
        break;
    default:
        break;
    }
    return strm;
}
