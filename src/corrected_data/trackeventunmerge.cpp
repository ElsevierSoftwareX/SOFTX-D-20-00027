#include "trackeventunmerge.h"

namespace CellTracker {

TrackEventUnmerge::TrackEventUnmerge() : TrackEvent(EVENT_TYPE_UNMERGE)
{
}
std::shared_ptr<Tracklet> TrackEventUnmerge::getPrev() const
{
    return prev;
}

void TrackEventUnmerge::setPrev(const std::shared_ptr<Tracklet> &value)
{
    prev = value;
}
QList<std::shared_ptr<Tracklet> > TrackEventUnmerge::getNext() const
{
    return next;
}

void TrackEventUnmerge::setNext(const QList<std::shared_ptr<Tracklet> > &value)
{
    next = value;
}

}
