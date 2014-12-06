#ifndef TRACKEVENTLOST_H
#define TRACKEVENTLOST_H

#include "tracklet.h"
#include "trackevent.h"

/*!
 * \brief The TrackEventLost class
 *
 * TrackEventLost is intended for the situation in which an object ist lost (and
 * doesn't reapper or cannot realiably be assigned to an existing Tracklet.
 *
 * Consequently this TrackEvent only holds the previous Tracklet.
 */
class TrackEventLost : public TrackEvent
{
public:
    TrackEventLost();

private:
    Tracklet prev; /*!< The previous Tracklet, which is now lost */
};

#endif // TRACKEVENTLOST_H
