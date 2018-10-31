/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2017 Sebastian Wagner
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
#ifndef TRACKEVENTLOST_H
#define TRACKEVENTLOST_H

#include "trackevent.h"

#include <memory>

#include "tracklet.h"

namespace CellTracker {

/*!
 * \brief The TrackEventLost class
 *
 * TrackEventLost is intended for the situation in which an object ist lost (and
 * doesn't reapper or cannot realiably be assigned to an existing Tracklet.
 *
 * Consequently this TrackEvent only holds the previous Tracklet.
 */
template <typename T>
class TrackEventLost : public TrackEvent<T>
{
    friend std::ostream& ::operator<< <>(std::ostream&, CellTracker::TrackEvent<T>&);
public:
    /*!
     * \brief constructs a new TrackEventLost
     */
    TrackEventLost() : TrackEvent<T>(TrackEvent<T>::EVENT_TYPE_LOST) {}


    /*!
     * \brief returns the previous Tracklet
     */
    std::weak_ptr<Tracklet> getPrev() const {
        return prev;
    }
    /*!
     * \brief sets the previous Tracklet
     * \param value the previous Tracklet
     */
    void setPrev(const std::weak_ptr<Tracklet> &value) {
        prev = value;
    }

private:
    std::weak_ptr<Tracklet> prev; /*!< The previous Tracklet, which is now lost */
};

}

#endif // TRACKEVENTLOST_H
