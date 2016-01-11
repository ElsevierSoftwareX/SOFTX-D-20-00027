#include "base/autotracklet.h"
#include "trackevent.h"
#include "trackeventdead.h"
#include "trackeventdivision.h"
#include "trackeventlost.h"
#include "trackeventmerge.h"
#include "trackeventunmerge.h"

#include <QDebug>
namespace CellTracker {

/*!
 * \brief constructor for TrackEvent
 */
template <typename T>
TrackEvent<T>::TrackEvent() {}

/*!
 * \brief constructor for TrackEvent
 * params t the type of the TrackEvent
 */
template <typename T>
TrackEvent<T>::TrackEvent(TrackEvent<T>::EVENT_TYPE t) :
    type(t) {}

/*!
 * \brief returns the type of the TrackEvent
 * \return the type of the TrackEvent
 */
template <typename T>
typename TrackEvent<T>::EVENT_TYPE
TrackEvent<T>::getType() const
{
   return this->type;
}

/* Templates suck. See
 * https://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor
 */
template class TrackEvent<AutoTracklet>;
template class TrackEvent<Tracklet>;

}

template <typename T>
std::ostream &
operator<<(std::ostream &strm, CellTracker::TrackEvent<T> &t)
{
    strm << "TrackEvent: ";
    switch (t.type) {
    case CellTracker::TrackEvent<T>::EVENT_TYPE_DEAD: {
        strm << "[EVENT_TYPE_DEAD]" << std::endl;
        CellTracker::TrackEventDead<T> &ted = static_cast<CellTracker::TrackEventDead<T>&>(t);
        strm << "prev: " << ted.prev << std::endl; }
        break;
    case CellTracker::TrackEvent<T>::EVENT_TYPE_DIVISION: {
        strm << "[EVENT_TYPE_DIVISION]" << std::endl;
        CellTracker::TrackEventDivision<T> &ted = static_cast<CellTracker::TrackEventDivision<T>&>(t);
        strm << "prev: " << ted.prev << std::endl;
        strm << "next: ";
        for (std::shared_ptr<CellTracker::Tracklet> n: *ted.next)
            strm << n->getId() << " ";
        strm << std::endl; }
        break;
    case CellTracker::TrackEvent<T>::EVENT_TYPE_LOST: {
        strm << "[EVENT_TYPE_LOST]" << std::endl;
        CellTracker::TrackEventLost<T> &tel = static_cast<CellTracker::TrackEventLost<T>&>(t);
        strm << "prev: " << tel.prev << std::endl; }
        break;
    case CellTracker::TrackEvent<T>::EVENT_TYPE_MERGE: {
        strm << "[EVENT_TYPE_MERGE]" << std::endl;
        CellTracker::TrackEventMerge<T> &tem = static_cast<CellTracker::TrackEventMerge<T>&>(t);
        strm << "prev: ";
        for (std::shared_ptr<CellTracker::Tracklet> p: *tem.prev)
            strm << p->getId() << " ";
        strm << std::endl;
        strm << "next: " << tem.next << std::endl; }
        break;
    case CellTracker::TrackEvent<T>::EVENT_TYPE_UNMERGE: {
        strm << "[EVENT_TYPE_UNMERGE]" << std::endl;
        CellTracker::TrackEventUnmerge<T> &teu = static_cast<CellTracker::TrackEventUnmerge<T>&>(t);
        strm << "prev: " << teu.prev << std::endl;
        strm << "next: ";
        for (std::shared_ptr<CellTracker::Tracklet> n: *teu.next)
            strm << n->getId() << " ";
        strm << std::endl; }
        break;
    default:
        break;
    }
    return strm;
}
