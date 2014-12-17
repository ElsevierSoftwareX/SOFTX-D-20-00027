#ifndef TRACKEVENT_H
#define TRACKEVENT_H

#include "trackelement.h"

#include <iostream>
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
class TrackEvent : public TrackElement
{
    friend std::ostream& operator<< (std::ostream&, TrackEvent&);
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

    TrackEvent();
    TrackEvent(EVENT_TYPE);

private:
    EVENT_TYPE type; /*!< The concrete type of this TrackEvent */
};

std::ostream& operator<< (std::ostream&, TrackEvent&);

#endif // TRACKEVENT_H
