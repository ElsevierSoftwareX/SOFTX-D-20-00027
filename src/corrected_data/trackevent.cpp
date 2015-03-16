#include "trackevent.h"
#include "trackeventdead.h"
#include "trackeventdivision.h"
#include "trackeventlost.h"
#include "trackeventmerge.h"
#include "trackeventunmerge.h"

#include <QDebug>
namespace CellTracker {

TrackEvent::TrackEvent() : TrackElement(ELEMENT_EVENT)
{
}

TrackEvent::TrackEvent(TrackEvent::EVENT_TYPE t) : TrackElement(ELEMENT_EVENT)
{
    this->type = t;
}

TrackEvent::EVENT_TYPE TrackEvent::getType() const
{
   return this->type;
}

}

std::ostream &operator<<(std::ostream &strm, CellTracker::TrackEvent &t)
{
    strm << "TrackEvent: ";
    switch (t.type) {
    case CellTracker::TrackEvent::EVENT_TYPE_DEAD: {
        strm << "[EVENT_TYPE_DEAD]" << std::endl;
        CellTracker::TrackEventDead &ted = static_cast<CellTracker::TrackEventDead&>(t);
        strm << "prev: " << ted.prev << std::endl; }
        break;
    case CellTracker::TrackEvent::EVENT_TYPE_DIVISION: {
        strm << "[EVENT_TYPE_DIVISION]" << std::endl;
        CellTracker::TrackEventDivision &ted = static_cast<CellTracker::TrackEventDivision&>(t);
        strm << "prev: " << ted.prev << std::endl;
        strm << "next: ";
        for (std::shared_ptr<CellTracker::Tracklet> n: *ted.next)
            strm << n->getID() << " ";
        strm << std::endl; }
        break;
    case CellTracker::TrackEvent::EVENT_TYPE_LOST: {
        strm << "[EVENT_TYPE_LOST]" << std::endl;
        CellTracker::TrackEventLost &tel = static_cast<CellTracker::TrackEventLost&>(t);
        strm << "prev: " << tel.prev << std::endl; }
        break;
    case CellTracker::TrackEvent::EVENT_TYPE_MERGE: {
        strm << "[EVENT_TYPE_MERGE]" << std::endl;
        CellTracker::TrackEventMerge &tem = static_cast<CellTracker::TrackEventMerge&>(t);
        strm << "prev: ";
        for (std::shared_ptr<CellTracker::Tracklet> p: tem.prev)
            strm << p->getID() << " ";
        strm << std::endl;
        strm << "next: " << tem.next << std::endl; }
        break;
    case CellTracker::TrackEvent::EVENT_TYPE_UNMERGE: {
        strm << "[EVENT_TYPE_UNMERGE]" << std::endl;
        CellTracker::TrackEventUnmerge &teu = static_cast<CellTracker::TrackEventUnmerge&>(t);
        strm << "prev: " << teu.prev << std::endl;
        strm << "next: ";
        for (std::shared_ptr<CellTracker::Tracklet> n: teu.next)
            strm << n->getID() << " ";
        strm << std::endl; }
        break;
    default:
        break;
    }
    return strm;
}
