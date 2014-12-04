#ifndef TRACKEVENTMERGE_H
#define TRACKEVENTMERGE_H

#include <QList>

#include "tracklet.h"
#include "trackevent.h"

class TrackEventMerge : public TrackEvent
{
public:
    TrackEventMerge();

private:
    QList<Tracklet> prev;
    Tracklet next;
};

#endif // TRACKEVENTMERGE_H
