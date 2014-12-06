#ifndef TRACKEVENTDEAD_H
#define TRACKEVENTDEAD_H

#include "tracklet.h"
#include "trackevent.h"

/*!
 * \brief The TrackEventDead class
 *
 * This class should be used, when a tracked object is dead, ie. doesn't move
 * anymore and/or dissolves.
 *
 * Consequently there is no member next, but only prev.
 */
class TrackEventDead : public TrackEvent
{
public:
    TrackEventDead();

private:
    Tracklet prev; /*!< The previous Tracklet whose Object is now dead */
};

#endif // TRACKEVENTDEAD_H
