#include "genealogy.h"
#include "trackeventdivision.h"

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

int Genealogy::removeTracklet(int id)
{
    return tracklets.remove(id);
}

std::shared_ptr<QList<std::shared_ptr<Annotation> > > Genealogy::getAnnotations() const
{
    return annotations;
}

void Genealogy::setAnnotations(const std::shared_ptr<QList<std::shared_ptr<Annotation>>> &value)
{
    annotations = value;
}

void Genealogy::addAnnotation(std::shared_ptr<Annotateable> annotated, std::string annotation)
{
    std::shared_ptr<Annotation> a = std::shared_ptr<Annotation>(new Annotation());
    a->setAnnotationText(annotation);
    a->setAnnotated(annotated);
    this->annotations->append(a);
}

/*!
 * \brief adds an Division-Event to the mother tracklet
 * \param motherId
 * \return true, if successfull, false if the mother could not be found
 */
bool Genealogy::addDivision(int motherId)
{
    std::shared_ptr<Tracklet> mother = this->getTracklet(motherId);

    if (mother) {
        std::shared_ptr<TrackEventDivision> divEv = std::shared_ptr<TrackEventDivision>(new TrackEventDivision());
        mother->setNext(divEv);
        return true;
    } else {
        return false;
    }
}

/*!
 * \brief adds a daughter-tracklet to the mother-tracklets division-event
 * \param motherId
 * \param daughterId
 * \return true if successfull, false if mother or daughter could not be found or if the event following the mother tracklet is not TrackEventDivision
 */
bool Genealogy::addDaughterTrack(int motherId, int daughterId)
{
   std::shared_ptr<Tracklet> mother = this->getTracklet(motherId);
   std::shared_ptr<Tracklet> daughter = this->getTracklet(daughterId);

   if (mother && daughter) {
       std::shared_ptr<TrackEvent> ev = mother->getNext();
       if (ev && ev->getType() == TrackEvent::EVENT_TYPE_DIVISION) {
           std::shared_ptr<TrackEventDivision> divEv = std::static_pointer_cast<TrackEventDivision>(ev);
           /*! \todo Do this without copying */
           auto nxt = divEv->getNext();
           nxt.append(daughter);
           divEv->setNext(nxt);
           return true;
       } else {
           return false;
       }
   } else {
       return false;
   }
}

}

std::ostream &operator<<(std::ostream &strm, CellTracker::Genealogy &g)
{
    strm << "Genealogy:" << std::endl;
    strm << "  annotations:" << std::endl;
    for (std::shared_ptr<CellTracker::Annotation> a: *(g.annotations))
        strm << "    " << *a;
    strm << "  tracklets:" << std::endl;
    for (std::shared_ptr<CellTracker::Tracklet> t: g.tracklets)
        strm << "    " << *t;
    return strm;
}
