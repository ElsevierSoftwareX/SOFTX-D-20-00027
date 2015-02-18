#include "trackeventmerge.h"

namespace CellTracker {

TrackEventMerge::TrackEventMerge() : TrackEvent(EVENT_TYPE_MERGE)
{
}
QList<std::shared_ptr<Tracklet> > TrackEventMerge::getPrev() const
{
    return prev;
}

void TrackEventMerge::setPrev(const QList<std::shared_ptr<Tracklet> > &value)
{
    prev = value;
}
std::shared_ptr<Tracklet> TrackEventMerge::getNext() const
{
    return next;
}

void TrackEventMerge::setNext(const std::shared_ptr<Tracklet> &value)
{
    next = value;
}

}
