#include "trackeventdead.h"

TrackEventDead::TrackEventDead() : TrackEvent(EVENT_TYPE_DEAD)
{
}
std::shared_ptr<Tracklet> TrackEventDead::getPrev() const
{
    return prev;
}

void TrackEventDead::setPrev(const std::shared_ptr<Tracklet> &value)
{
    prev = value;
}

