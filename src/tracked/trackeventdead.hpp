/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2017 Sebastian Wagner
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
#ifndef TRACKEVENTDEAD_H
#define TRACKEVENTDEAD_H

#include "trackevent.h"

#include <memory>

#include "tracklet.h"

namespace TraCurate {

/*!
 * \brief The TrackEventDead class
 *
 * This class should be used, when a tracked object is dead, ie. doesn't move
 * anymore and/or dissolves.
 *
 * Consequently there is no member next, but only prev.
 */
template <typename T>
class TrackEventDead : public TrackEvent<T>
{
    friend std::ostream& ::operator<< <>(std::ostream&, TraCurate::TrackEvent<T>&);
public:
    /*!
     * \brief constructs a new TrackEventDead
     */
    TrackEventDead() : TrackEvent<T>(TrackEvent<T>::EVENT_TYPE_DEAD) {}

   /*!
    * \brief returns the previous Tracklet
    * \return the previous Tracklet
    */
    std::weak_ptr<T> getPrev() const {
        return prev;
    }
    /*!
     * \brief sets the previous Tracklet
     * \param value the Tracklet to set as previous
     */
    void setPrev(const std::weak_ptr<T> &value) {
        prev = value;
    }

private:
    std::weak_ptr<T> prev; /*!< The previous Tracklet whose Object is now dead */
};

}

#endif // TRACKEVENTDEAD_H
