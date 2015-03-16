#include "trackeventdivision.h"

namespace CellTracker {

TrackEventDivision::TrackEventDivision() : TrackEvent(EVENT_TYPE_DIVISION)
{
}
std::shared_ptr<Tracklet> TrackEventDivision::getPrev() const
{
    return prev;
}

void TrackEventDivision::setPrev(const std::shared_ptr<Tracklet> &value)
{
    prev = value;
}
std::shared_ptr<QList<std::shared_ptr<Tracklet>>> TrackEventDivision::getNext() const
{
    return next;
}

void TrackEventDivision::setNext(const std::shared_ptr<QList<std::shared_ptr<Tracklet>>> &value)
{
    next = value;
}

}
