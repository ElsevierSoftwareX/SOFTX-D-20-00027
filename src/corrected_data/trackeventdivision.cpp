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
QList<std::shared_ptr<Tracklet> > TrackEventDivision::getNext() const
{
    return next;
}

void TrackEventDivision::setNext(const QList<std::shared_ptr<Tracklet> > &value)
{
    next = value;
}

}
