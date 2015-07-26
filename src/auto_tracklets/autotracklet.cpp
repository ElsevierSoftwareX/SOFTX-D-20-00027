#include "autotracklet.h"

#include <QDebug>

namespace CellTracker {

AutoTracklet::AutoTracklet()
{
}

void AutoTracklet::addComponent(std::shared_ptr<Frame> f,std::shared_ptr<Object> o)
{
    QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> pair(f,o);
    o->setAutoId(this->getID());
    this->addComponent(pair);
}

void AutoTracklet::addComponent(QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>> p)
{
    this->components.append(p);
}

void AutoTracklet::setID(int id)
{
    this->trackID = id;
}

int AutoTracklet::getID()
{
    return this->trackID;
}

QList<QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>>> AutoTracklet::getComponents() const
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

QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>> AutoTracklet::getStart() {
    qSort(components.begin(), components.end(),
          [](const QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> a, const QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> b) -> bool {
                return (a.first->getID() != b.first->getID())?
                            (a.first->getID() < b.first->getID()):
                            (a.second->getId() < b.second->getId());
          });
    return components.first();
}

QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>> AutoTracklet::getEnd() {
    qSort(components.begin(), components.end(),
          [](const QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> a, const QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> b) -> bool {
                return (a.first->getID() != b.first->getID())?
                            (a.first->getID() < b.first->getID()):
                            (a.second->getId() < b.second->getId());
          });
    return components.last();
}

uint32_t AutoTracklet::getLength() {
    return getEnd().first->getID()-getStart().first->getID();
}

}

std::ostream &operator<< (std::ostream &strm, CellTracker::AutoTracklet &a)
{
    strm << "    AutoTracklet:" << std::endl;
    strm << "      id: " << a.trackID << std::endl;
    strm << "      components:" << std::endl << "          ";
    for (QPair<std::shared_ptr<CellTracker::Frame>,std::shared_ptr<CellTracker::Object>> &p: a.components)
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
