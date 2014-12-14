#include "autotracklet.h"

AutoTracklet::AutoTracklet()
{
}

void AutoTracklet::addComponent(std::shared_ptr<Frame> f,std::shared_ptr<Object> o)
{
    QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> pair(f,o);
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

std::ostream &operator<< (std::ostream &strm, AutoTracklet &a)
{
    strm << "    AutoTracklet:\n";
    strm << "      id: " << a.trackID << "\n";
    strm << "      components:\n          ";
    for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> p: a.components)
        strm << p;
    strm << "\n";
    return strm;
}

/* Qt doesn't provide operator<< to ostream */
std::ostream &operator<<(std::ostream &strm, QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>> &p)
{
    strm << "(" << p.first->getID() << "," << p.second->getID() << ")";
    return strm;
}
