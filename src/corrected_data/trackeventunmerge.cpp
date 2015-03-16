#include "trackeventunmerge.h"

namespace CellTracker {

TrackEventUnmerge::TrackEventUnmerge() : TrackEvent(EVENT_TYPE_UNMERGE)
{
    this->next = std::shared_ptr<QList<std::shared_ptr<Tracklet>>>(new QList<std::shared_ptr<Tracklet>>());
}
std::shared_ptr<Tracklet> TrackEventUnmerge::getPrev() const
{
    return prev;
}

void TrackEventUnmerge::setPrev(const std::shared_ptr<Tracklet> &value)
{
    prev = value;
}
std::shared_ptr<QList<std::shared_ptr<Tracklet>>> TrackEventUnmerge::getNext() const
{
    return next;
}

void TrackEventUnmerge::setNext(const std::shared_ptr<QList<std::shared_ptr<Tracklet>>> &value)
{
    next = value;
}

}
