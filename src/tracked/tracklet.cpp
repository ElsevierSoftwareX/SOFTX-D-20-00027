#include "provider/idprovider.h"
#include "tracklet.h"

#include <functional>

namespace CellTracker {

Tracklet::Tracklet() : TrackElement(ELEMENT_TRACKLET), Annotateable(TRACKLET_ANNOTATION)
{
    this->id = IdProvider::getNewTrackletId();
}

Tracklet::~Tracklet()
{
    IdProvider::returnTrackletId(this->id);
}

QList<QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> Tracklet::getObjectsAt(int frameId) const
{
    return this->contained.values(frameId);
}

QHash<int,QPair<std::shared_ptr<Frame>, std::shared_ptr<Object> > > Tracklet::getContained() const
{
    return contained;
}

bool Tracklet::hasObjectAt(int objId, int frameId) {
    QPair<int,int> p(frameId, objId);
    return contained.contains(qHash<int,int>(p));
}

/*!
 * \brief Tracklet::setContained
 * \param value the new value
 */
void Tracklet::setContained(const QHash<int,QPair<std::shared_ptr<Frame>, std::shared_ptr<Object> > > &value)
{
    contained = value;
}

/*!
 * \brief Tracklet::addToContained
 * \param f the frame
 * \param o the object
 */
void Tracklet::addToContained(const std::shared_ptr<Frame> &f, const std::shared_ptr<Object> &o)
{
    QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> pair(f,o);
    this->addToContained(pair);
}

void Tracklet::addToContained(const QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>> p)
{
    QPair<int,int> idPair (p.first->getID(), p.second->getId());
    p.second->setTrackId(this->id);
    this->contained.insert(qHash<int,int>(idPair), p);
}

void Tracklet::removeFromContained(int frameId, uint32_t objId)
{
    QPair<int,int> idPair (frameId, objId);
    this->contained.value(qHash<int,int>(idPair)).second->setTrackId(UINT32_MAX);
    this->contained.remove(qHash<int,int>(idPair));
}

QPair<std::shared_ptr<Frame>, std::shared_ptr<Object> > Tracklet::getEnd() const
{
    QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> ret(nullptr,nullptr);
    for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> p: this->contained) {
        if (ret.first == nullptr || p.first->getID() > ret.first->getID())
            ret = p;
    }
    return ret;
}

QPair<std::shared_ptr<Frame>, std::shared_ptr<Object> > Tracklet::getStart() const
{
    QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> ret(nullptr,nullptr);
    for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> p: this->contained) {
        if (ret.first == nullptr || p.first->getID() < ret.first->getID())
            ret = p;
    }
    return ret;
}

std::shared_ptr<TrackEvent<Tracklet> > Tracklet::getNext() const
{
    return next;
}

std::shared_ptr<TrackEvent<Tracklet> > Tracklet::getPrev() const
{
    return prev;
}

void Tracklet::setNext(const std::shared_ptr<TrackEvent<Tracklet> > &value)
{
    next = value;
}

void Tracklet::setPrev(const std::shared_ptr<TrackEvent<Tracklet> > &value)
{
    prev = value;
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
    strm << std::endl;
    strm << "  id: " << t.id << std::endl;
    strm << "  next: " << t.next << std::endl;
    strm << "  contained: ";
    for (QPair<std::shared_ptr<CellTracker::Frame>,std::shared_ptr<CellTracker::Object>> p: t.contained) {
        strm << "(" << p.first->getID() << "," << p.second->getId() << ") ";
    }
    strm << std::endl;
    return strm;
}
