#ifndef TRACKEVENTDIVISION_H
#define TRACKEVENTDIVISION_H

#include <QList>

#include "tracklet.h"
#include "trackevent.h"

class TrackEventDivision : public TrackEvent
{
public:
    TrackEventDivision();

private:
    Tracklet prev;
    QList<Tracklet> next;
};

#endif // TRACKEVENTDIVISION_H
