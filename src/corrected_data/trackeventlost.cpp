#include "trackeventlost.h"

namespace CellTracker {

TrackEventLost::TrackEventLost() : TrackEvent(EVENT_TYPE_LOST)
{
}
std::shared_ptr<Tracklet> TrackEventLost::getPrev() const
{
    return prev;
}

void TrackEventLost::setPrev(const std::shared_ptr<Tracklet> &value)
{
    prev = value;
}

}
