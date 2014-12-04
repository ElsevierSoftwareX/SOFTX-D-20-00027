#ifndef TRACKLETSPLIT_H
#define TRACKLETSPLIT_H

#include "tracklet.h"
#include "../auto_tracklets/autotracklet.h"
#include "../base_data/frame.h"

class TrackletSplit : public Tracklet
{
public:
    TrackletSplit();

private:
    AutoTracklet baseTracklet;
    Frame from;
    Frame to;
};

#endif // TRACKLETSPLIT_H
