#ifndef TRACKLETMERGED_H
#define TRACKLETMERGED_H

#include <QList>

#include "tracklet.h"
#include "../auto_tracklets/autotracklet.h"

class TrackletMerged : public Tracklet
{
public:
    TrackletMerged();

private:
    QList<AutoTracklet> containedTracklets;
};

#endif // TRACKLETMERGED_H
