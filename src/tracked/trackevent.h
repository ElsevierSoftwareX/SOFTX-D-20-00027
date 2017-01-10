#ifndef TRACKEVENT_H
#define TRACKEVENT_H

#include <iostream>

namespace CellTracker { template <typename T> class TrackEvent; }
template <typename T> std::ostream& operator<< (std::ostream&, CellTracker::TrackEvent<T>&);

namespace CellTracker {

/*!
 * \brief The TrackEvent class
 *
 * The TrackEvent-class is superclass to all the possible TrackEvent%s. Those
 * include
 *  - TrackEventDivision
 *  - TrackEventMerge
 *  - TrackEventUnmerge
 *  - TrackEventLost
 *  - TrackEventDead and
 *  - TrackEventEndOfMovie.
 *
 * The concrete type should be set using the enum EVENT_TYPE.
 */
template <typename T>
class TrackEvent
{
    friend std::ostream& ::operator<< <>(std::ostream&, CellTracker::TrackEvent<T>&);

public:
    /*!
     * \brief The EVENT_TYPE enum
     *
     * This enum is used to specify the type of a TrackEvent.
     */
    enum EVENT_TYPE {
        EVENT_TYPE_DIVISION   = 0,  /*!< TrackEvent is of type TrackEventDivision */
        EVENT_TYPE_MERGE      = 1,  /*!< TrackEvent is of type TrackEventMerge */
        EVENT_TYPE_UNMERGE    = 2,  /*!< TrackEvent is of type TrackEventUnmerge */
        EVENT_TYPE_LOST       = 3,  /*!< TrackEvent is of type TrackEventLost */
        EVENT_TYPE_DEAD       = 4,  /*!< TrackEvent is of type TrackEventDead */
        EVENT_TYPE_ENDOFMOVIE = 5   /*!< TrackEvent is of type TrackEventEndOfMovie */
    };

    /*!
     * \brief constructor for TrackEvent
     * params t the type of the TrackEvent
     */
    TrackEvent(TrackEvent<T>::EVENT_TYPE t) : type(t) {}
    TrackEvent() __attribute__((deprecated)) = default;

    /*!
     * \brief returns the type of the TrackEvent
     * \return the type of the TrackEvent
     */
    EVENT_TYPE getType() const {
        return this->type;
    }

private:
    EVENT_TYPE type; /*!< The concrete type of this TrackEvent */
};

}

#endif // TRACKEVENT_H
