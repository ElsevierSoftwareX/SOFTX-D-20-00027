#include "genealogy.h"

namespace CellTracker {

Genealogy::Genealogy()
{
}

std::shared_ptr<Tracklet> Genealogy::getTracklet(int &id) const
{
    return tracklets.value(id,nullptr);
}

void Genealogy::addTracklet(const std::shared_ptr<Tracklet> &value)
{
    if (!tracklets.contains(value->getID()))
        tracklets.insert(value->getID(),value);
}
std::shared_ptr<QList<Annotation> > Genealogy::getAnnotations() const
{
    return annotations;
}

void Genealogy::setAnnotations(const std::shared_ptr<QList<Annotation> > &value)
{
    annotations = value;
}

}

std::ostream &operator<<(std::ostream &strm, CellTracker::Genealogy &g)
{
    strm << "Genealogy:" << std::endl;
    strm << "  annotations:" << std::endl;
    for (CellTracker::Annotation a: *(g.annotations))
        strm << "    " << a;
    strm << "  tracklets:" << std::endl;
    for (std::shared_ptr<CellTracker::Tracklet> t: g.tracklets)
        strm << "    " << *t;
    return strm;
}
