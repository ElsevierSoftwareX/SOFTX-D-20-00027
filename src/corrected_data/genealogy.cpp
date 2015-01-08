#include "genealogy.h"

Genealogy::Genealogy()
{
}

std::shared_ptr<Tracklet> Genealogy::getTracklet(int &id) const
{
    if(id >=0 && id < tracklets.size()){
        std::shared_ptr<Tracklet> t = tracklets.at(id);
        if(t->getID() == id)
            return t;
    }

    /* unordered list */
    for (std::shared_ptr<Tracklet> t: tracklets)
        if (t->getID() == id)
            return t;

    /* searched everything and found nothing */
    return nullptr;
}

void Genealogy::addTracklet(const std::shared_ptr<Tracklet> &value)
{
    tracklets.append(value);
}



std::ostream &operator<<(std::ostream &strm, Genealogy &g)
{
    strm << "Genealogy:" << std::endl;
    strm << "  annotations: [unimplemented]" << std::endl;
    strm << "  trackles:" << std::endl;
    for (std::shared_ptr<Tracklet> t: g.tracklets) {
        strm << "    " << *t;
    }
    return strm;
}
