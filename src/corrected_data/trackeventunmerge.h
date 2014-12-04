#ifndef TRACKEVENTUNMERGE_H
#define TRACKEVENTUNMERGE_H

#include <QList>

#include "tracklet.h"
#include "trackevent.h"

class TrackEventUnmerge : public TrackEvent
{
public:
    TrackEventUnmerge();

private:
    Tracklet prev;
    QList<Tracklet> next;
};

#endif // TRACKEVENTUNMERGE_H
