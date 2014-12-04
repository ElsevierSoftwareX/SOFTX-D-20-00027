#ifndef TRACKEVENTDEAD_H
#define TRACKEVENTDEAD_H

#include "tracklet.h"
#include "trackevent.h"

class TrackEventDead : public TrackEvent
{
public:
    TrackEventDead();

private:
    Tracklet prev;
};

#endif // TRACKEVENTDEAD_H
