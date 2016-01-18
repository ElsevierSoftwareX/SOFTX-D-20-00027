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
 *  - TrackEventLost and
 *  - TrackEventDead.
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
        EVENT_TYPE_DIVISION, /*!< TrackEvent is of type TrackEventDivision */
        EVENT_TYPE_MERGE,    /*!< TrackEvent is of type TrackEventMerge */
        EVENT_TYPE_UNMERGE,  /*!< TrackEvent is of type TrackEventUnmerge */
        EVENT_TYPE_LOST,     /*!< TrackEvent is of type TrackEventLost */
        EVENT_TYPE_DEAD      /*!< TrackEvent is of type TrackEventDead */
    };

    TrackEvent() __attribute__((deprecated));
    TrackEvent(TrackEvent<T>::EVENT_TYPE);

    EVENT_TYPE getType() const;

private:
    EVENT_TYPE type; /*!< The concrete type of this TrackEvent */
};

}

#endif // TRACKEVENT_H
