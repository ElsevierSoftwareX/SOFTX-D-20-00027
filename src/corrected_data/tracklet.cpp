#include "tracklet.h"
#include "trackletmerged.h"
#include "trackletregular.h"
#include "trackletsplit.h"

#include <functional>

namespace CellTracker {

Tracklet::Tracklet() : TrackElement(ELEMENT_TRACKLET), Annotateable(TRACKLET_ANNOTATION)
{
}

Tracklet::Tracklet(Tracklet::TRACKLET_TYPE t) : TrackElement(ELEMENT_TRACKLET), Annotateable(TRACKLET_ANNOTATION)
{
    this->type = t;
}

Tracklet::TRACKLET_TYPE Tracklet::getType() const
{
    return type;
}

void Tracklet::setType(const TRACKLET_TYPE &value)
{
    type = value;
}

QList<QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> Tracklet::getObjectsAt(int frameId) const
{
    return this->contained.values(frameId);
}

QHash<int,QPair<std::shared_ptr<Frame>, std::shared_ptr<Object> > > Tracklet::getContained() const
{
    return contained;
}

/*!
 * \brief Tracklet::setContained
 * \param value
 */
void Tracklet::setContained(const QHash<int,QPair<std::shared_ptr<Frame>, std::shared_ptr<Object> > > &value)
{
    contained = value;
}

/*!
 * \brief Tracklet::addToContained
 * \param f
 * \param o
 */
void Tracklet::addToContained(const std::shared_ptr<Frame> f, const std::shared_ptr<Object> o)
{
    QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> pair(f,o);
    QPair<int,int> idPair (f->getID(),o->getID());
    contained.insert(qHash<int,int>(idPair),pair);
}

void Tracklet::removeFromContained(int frameId, uint32_t objId)
{
    QPair<int,int> idPair (frameId, objId);
    this->contained.remove(qHash<int,int>(idPair));
}
std::shared_ptr<TrackEvent<Tracklet> > Tracklet::getNext() const
{
    return next;
}

void Tracklet::setNext(const std::shared_ptr<TrackEvent<Tracklet> > &value)
{
    next = value;
}


int Tracklet::getID() const
{
    return id;
}

void Tracklet::setID(int value)
{
    id = value;
}

}

std::ostream &operator<<(std::ostream &strm, CellTracker::Tracklet &t)
{
    strm << "Tracklet: ";
    switch (t.type) {
    case CellTracker::Tracklet::TRACKLET_MERGED:
        strm << "[TRACKLET_MERGED]";
        break;
    case CellTracker::Tracklet::TRACKLET_REGULAR:
        strm << "[TRACKLET_REGULAR]";
        break;
    case CellTracker::Tracklet::TRACKLET_SPLIT:
        strm << "[TRACKLET_SPLIT]";
    default:
        break;
    }
    strm << std::endl;
    strm << "  id: " << t.id << std::endl;
    strm << "  next: " << t.next << std::endl;
    strm << "  contained: ";
    for (QPair<std::shared_ptr<CellTracker::Frame>,std::shared_ptr<CellTracker::Object>> p: t.contained) {
        strm << "(" << p.first->getID() << "," << p.second->getID() << ") ";
    }
    strm << std::endl;
    switch (t.type) {
    case CellTracker::Tracklet::TRACKLET_MERGED: {
        CellTracker::TrackletMerged &tm = static_cast<CellTracker::TrackletMerged&>(t);
        strm << "  containedTracklets:" << std::endl;
        for (auto tr: tm.containedTracklets)
            strm << tr; }
        break;
    case CellTracker::Tracklet::TRACKLET_REGULAR: {
        CellTracker::TrackletRegular &tr = static_cast<CellTracker::TrackletRegular&>(t);
        strm << "  baseTracklet: " << tr.baseTracklet->getID() << std::endl; }
        break;
    case CellTracker::Tracklet::TRACKLET_SPLIT: {
        CellTracker::TrackletSplit &ts = static_cast<CellTracker::TrackletSplit&>(t);
        strm << "  baseTracklet: " << ts.baseTracklet->getID() << std::endl;
        strm << "  from: " << ts.from << std::endl;
        strm << "  to: " << ts.to << std::endl; }
       break;
    default:
        break;
    }
    return strm;
}
