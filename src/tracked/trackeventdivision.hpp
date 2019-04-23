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
#ifndef TRACKEVENTDIVISION_H
#define TRACKEVENTDIVISION_H

#include "trackevent.h"

#include <memory>

#include <QList>

#include "tracklet.h"

namespace TraCurate {

/*!
 * \brief The TrackEventDivision class
 *
 * TrackEventDivision is used in situations, when an Object actually divides
 * itself (e.g. cell division).
 *
 * Due to that it holds the previous Tracklet and a list of new Tracklets.
 */
template <typename T>
class TrackEventDivision : public TrackEvent<T>
{
    friend std::ostream& ::operator<< <>(std::ostream&, TraCurate::TrackEvent<T>&);
public:
    /*!
     * \brief constructs a new TrackEventDivision
     */
    TrackEventDivision() : TrackEvent<T>(TrackEvent<T>::EVENT_TYPE_DIVISION),
                           next(std::make_shared<QList<std::weak_ptr<T>>>()) {}

    /*!
     * \brief returns the previous (Auto)Tracklet
     * \return the previous (Auto)Tracklet
     */
    std::weak_ptr<T> getPrev() const {
        return prev;
    }
    /*!
     * \brief sets the previous (Auto)Tracklet
     * \param value the previous (Auto)Tracklet to set
     */
    void setPrev(const std::weak_ptr<T> &value) {
        prev = value;
    }

    /*!
     * \brief returns a QList of next (Auto)Tracklet%s
     * \return a Qlist of next (Auto)Tracklet%s
     */
    std::shared_ptr<QList<std::weak_ptr<T> > > getNext() const {
        return next;
    }
    /*!
     * \brief sets the QList of next (Auto)Tracklet%s
     * \param value the QList of next (Auto)Tracklet%s to set
     */
    void setNext(const std::shared_ptr<QList<std::weak_ptr<T>>> &value) {
        next = value;
    }

private:
    std::weak_ptr<T> prev;                         /*!< The previous Tracklet */
    std::shared_ptr<QList<std::weak_ptr<T>>> next; /*!< The list of next Tracklets */
};

}
#endif // TRACKEVENTDIVISION_H
