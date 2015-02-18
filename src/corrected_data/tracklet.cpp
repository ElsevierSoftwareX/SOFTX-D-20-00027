#include "tracklet.h"

#include <functional>

Tracklet::Tracklet() : TrackElement(ELEMENT_TRACKLET)
{
}

Tracklet::Tracklet(Tracklet::TRACKLET_TYPE t) : TrackElement(ELEMENT_TRACKLET)
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
QHash<int,QPair<std::shared_ptr<Frame>, std::shared_ptr<Object> > > Tracklet::getContained() const
{
    return contained;
}

void Tracklet::setContained(const QHash<int,QPair<std::shared_ptr<Frame>, std::shared_ptr<Object> > > &value)
{
    contained = value;
}

void Tracklet::addToContained(const std::shared_ptr<Frame> f, const std::shared_ptr<Object> o)
{
    QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> pair(f,o);
    QPair<int,int> idPair (f->getID(),o->getID());
    contained.insert(qHash<int,int>(idPair),pair);
}
std::shared_ptr<TrackEvent> Tracklet::getNext() const
{
    return next;
}

void Tracklet::setNext(const std::shared_ptr<TrackEvent> &value)
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

std::ostream &operator<<(std::ostream &strm, Tracklet &t)
{
    strm << "Tracklet:" << std::endl;
    strm << "  next: " << t.next << std::endl;
    strm << "  contained: ";
    for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> p: t.contained) {
        strm << "(" << p.first->getID() << "," << p.second->getID() << ") ";
    }
    strm << std::endl;
    return strm;
}
