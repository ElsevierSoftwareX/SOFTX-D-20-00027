#ifndef TRACKLETREGULAR_H
#define TRACKLETREGULAR_H

#include "tracklet.h"
#include "../auto_tracklets/autotracklet.h"

class TrackletRegular : public Tracklet
{
public:
    TrackletRegular();

private:
    AutoTracklet baseTracklet;
};

#endif // TRACKLETREGULAR_H
