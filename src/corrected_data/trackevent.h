#ifndef TRACKEVENT_H
#define TRACKEVENT_H

#include "trackelement.h"

class TrackEvent : public TrackElement
{
    enum EVENT_TYPE {
        EVENT_TYPE_DIVISION,
        EVENT_TYPE_MERGE,
        EVENT_TYPE_UNMERGE,
        EVENT_TYPE_LOST,
        EVENT_TYPE_DEAD
    };

public:
    TrackEvent();

private:
    EVENT_TYPE type;
};

#endif // TRACKEVENT_H
