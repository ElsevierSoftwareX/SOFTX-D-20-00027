#include "autotracklet.h"

#include <QDebug>

namespace CellTracker {

AutoTracklet::AutoTracklet() :
    trackID(UINT32_MAX) {}

void AutoTracklet::addComponent(std::shared_ptr<Frame> f,std::shared_ptr<Object> o)
{
    QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> pair(f,o);
    o->setAutoId(this->getID());
    this->addComponent(pair);
}

void AutoTracklet::addComponent(QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>> p)
{
    this->components.insert(p.first->getID(), p.second);
}

void AutoTracklet::setID(int id)
{
    this->trackID = id;
}

int AutoTracklet::getID()
{
    return this->trackID;
}

QMap<int, std::shared_ptr<Object>> AutoTracklet::getComponents() const
{
    return this->components;
}
std::shared_ptr<TrackEvent<AutoTracklet> > AutoTracklet::getNext() const
{
    return next;
}

void AutoTracklet::setNext(const std::shared_ptr<TrackEvent<AutoTracklet> > &value)
{
    next = value;
}

std::shared_ptr<TrackEvent<AutoTracklet> > AutoTracklet::getPrev() const
{
    return prev;
}

void AutoTracklet::setPrev(const std::shared_ptr<TrackEvent<AutoTracklet> > &value)
{
    prev = value;
}

int AutoTracklet::getStart() {
    return components.firstKey();
}

int AutoTracklet::getEnd() {
    return components.lastKey();
}

uint32_t AutoTracklet::getLength() {
    return getEnd()-getStart();
}

}

std::ostream &operator<< (std::ostream &strm, CellTracker::AutoTracklet &a)
{
    strm << "    AutoTracklet:" << std::endl;
    strm << "      id: " << a.trackID << std::endl;
    strm << "      components:" << std::endl << "          ";
    for (auto p : a.components)
        strm << p;
    strm << std::endl;
    return strm;
}

/* Qt doesn't provide operator<< to ostream */
std::ostream &operator<<(std::ostream &strm, QPair<std::shared_ptr<CellTracker::Frame>, std::shared_ptr<CellTracker::Object>> &p)
{
    strm << "(" << p.first->getID() << "," << p.second->getId() << ")";
    return strm;
}
