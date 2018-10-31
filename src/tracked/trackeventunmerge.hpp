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
#ifndef TRACKEVENTUNMERGE_H
#define TRACKEVENTUNMERGE_H

#include "trackevent.h"

#include <memory>

#include <QList>

#include "tracklet.h"

namespace CellTracker {

/*!
 * \brief The TrackEventUnmerge class
 *
 * The TrackEventUnmerge class should be used, when two previously merged Tracklet%s
 * (see TrackEventMerge) become separate tracks again.
 *
 * An instance of this class holds a Tracklet prev which is the previous
 * Tracklet and a QList of Tracklet%s that are the following Tracklets.
 */
template <typename T>
class TrackEventUnmerge : public TrackEvent<T>
{
    friend std::ostream& ::operator<< <>(std::ostream&, CellTracker::TrackEvent<T>&);
public:
    /*!
     * \brief constructs a new TrackEventUnmerge
     */
    TrackEventUnmerge() : TrackEvent<T>(TrackEvent<T>::EVENT_TYPE_UNMERGE),
                          next(std::make_shared<QList<std::weak_ptr<T>>>()) {}

    /*!
     * \brief returns the previous Tracklet
     * \return the previous Tracklet
     */
    std::weak_ptr<T> getPrev() const {
        return prev;
    }
    /*!
     * \brief sets the previous Tracklet
     * \param value the previous Tracklet to set
     */
    void setPrev(const std::weak_ptr<T> &value) {
        prev = value;
    }

    /*!
     * \brief returns a QList of next Tracklet%s
     * \return a QList of next Tracklet%s
     */
    std::shared_ptr<QList<std::weak_ptr<T>>> getNext() const {
        return next;
    }
    /*!
     * \brief sets the QList of next Tracklet%s
     * \param value the QList of next Tracklet%s to set
     */
    void setNext(const std::shared_ptr<QList<std::weak_ptr<T> > > &value) {
        next = value;
    }

private:
    std::weak_ptr<T> prev;                         /*!< The previously merged Track */
    std::shared_ptr<QList<std::weak_ptr<T>>> next; /*!< The unmerged Track%s */
};

}

#endif // TRACKEVENTUNMERGE_H
