#ifndef TRACKEVENTLOST_H
#define TRACKEVENTLOST_H

#include "tracklet.h"
#include "trackevent.h"

class TrackEventLost : public TrackEvent
{
public:
    TrackEventLost();

private:
    Tracklet prev;
};

#endif // TRACKEVENTLOST_H
