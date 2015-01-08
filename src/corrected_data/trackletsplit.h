#ifndef TRACKLETSPLIT_H
#define TRACKLETSPLIT_H

#include "tracklet.h"
#include "../auto_tracklets/autotracklet.h"
#include "../base_data/frame.h"

/*!
 * \brief The TrackletSplit class
 *
 * TrackletSplit describes the situation, when an AutoTracklet that was
 * automatically recognized is in fact one or more Tracklets.
 */
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