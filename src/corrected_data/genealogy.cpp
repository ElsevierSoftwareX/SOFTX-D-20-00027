#include "auto_tracklets/autotracklet.h"
#include "genealogy.h"
#include "trackeventdead.h"
#include "trackeventdivision.h"
#include "trackeventlost.h"
#include "trackeventmerge.h"
#include "trackeventunmerge.h"
#include "tracklet.h"

namespace CellTracker {

Genealogy::Genealogy(std::shared_ptr<Movie> m)
{
    this->annotations = std::shared_ptr<QList<std::shared_ptr<Annotation>>>(new QList<std::shared_ptr<Annotation>>());
    this->movie = m;
}

std::shared_ptr<Tracklet> Genealogy::getTracklet(int &id) const
{
    return tracklets.value(id,nullptr);
}

bool Genealogy::addTracklet(const std::shared_ptr<Tracklet> &value)
{
    if (tracklets.contains(value->getID()))
        return false;
    tracklets.insert(value->getID(),value);
    return true;
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

std::shared_ptr<Annotation> Genealogy::getAnnotation(std::shared_ptr<Annotateable> annotated)
{
    for (std::shared_ptr<Annotation> a: *annotations) {
        if (a->getAnnotated() == annotated)
            return a;
    }
    return nullptr;
}

void Genealogy::addAnnotation(std::shared_ptr<Annotateable> annotated, std::string annotation)
{
    std::shared_ptr<Annotation> a = std::shared_ptr<Annotation>(new Annotation());
    a->setAnnotationText(annotation);
    a->setAnnotated(annotated);
    this->annotations->append(a);
}

std::shared_ptr<Object> Genealogy::getObject(int trackId, int frameId, uint32_t objId) const
{
    std::shared_ptr<Object> ret;
    QList<QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> objs = this->getTracklet(trackId)->getObjectsAt(frameId);
    for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> p: objs)
        if (p.second->getID() == objId)
            return p.second;
    return nullptr;
}

void Genealogy::addObject(int frameId, int trackId, std::shared_ptr<Object> obj)
{
    std::shared_ptr<Frame> f = this->movie->getFrame(frameId);
    this->tracklets.value(trackId)->addToContained(f,obj);
}

void Genealogy::removeObject(int frameId, int trackId, uint32_t objId)
{
    this->tracklets.value(trackId)->removeFromContained(frameId, objId);
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
       std::shared_ptr<TrackEventDivision<Tracklet>> ev = std::static_pointer_cast<TrackEventDivision<Tracklet>>(mother->getNext());
       if (ev == nullptr) {
           /* No Event set, do that now */
           ev = std::shared_ptr<TrackEventDivision<Tracklet>>(new TrackEventDivision<Tracklet>());
           mother->setNext(ev);
       }
       if (ev->getType() == TrackEvent<Tracklet>::EVENT_TYPE_DIVISION) {
           ev->setPrev(mother);
           ev->getNext()->append(daughter);
           return true;
       }
   }
   /* Event_Type was not Divsion or mother/daughter could not be found*/
   return false;
}

bool Genealogy::setDead(int trackId)
{
    std::shared_ptr<Tracklet> t = this->tracklets.value(trackId,nullptr);
    if (t == nullptr)
        return false;
    t->setNext(std::shared_ptr<TrackEventDead<Tracklet>>(new TrackEventDead<Tracklet>()));
    return true;
}

bool Genealogy::setLost(int trackId)
{
    std::shared_ptr<Tracklet> t = this->tracklets.value(trackId,nullptr);
    if (t == nullptr)
        return false;
    t->setNext(std::shared_ptr<TrackEventLost<Tracklet>>(new TrackEventLost<Tracklet>()));
    return true;
}

bool Genealogy::setOpen(int trackId)
{
    std::shared_ptr<Tracklet> t = this->tracklets.value(trackId,nullptr);
    if (t == nullptr)
        return false;
    t->setNext(nullptr);
    return true;
}

bool Genealogy::addMerge(int prevId, int mergeId)
{
   std::shared_ptr<Tracklet> prev = this->getTracklet(prevId);
   std::shared_ptr<Tracklet> merge = this->getTracklet(mergeId);

   if (prev && merge) {
       std::shared_ptr<TrackEventMerge<Tracklet>> ev = std::static_pointer_cast<TrackEventMerge<Tracklet>>(prev->getNext());
       if (ev == nullptr) {
           /* No Event set, do that now */
           ev = std::shared_ptr<TrackEventMerge<Tracklet>>(new TrackEventMerge<Tracklet>());
           prev->setNext(ev);
       }
       if (ev->getType() == TrackEvent<Tracklet>::EVENT_TYPE_MERGE) {
           ev->getPrev()->append(prev);
           ev->setNext(merge);
           return true;
       }
   }
   /* Event_Type was not Merge or prev/merge could not be found */
   return false;

}

bool Genealogy::addUnmerge(int mergeId, int nextId)
{
   std::shared_ptr<Tracklet> merge = this->getTracklet(mergeId);
   std::shared_ptr<Tracklet> next = this->getTracklet(nextId);

   if (merge && next) {
       std::shared_ptr<TrackEventUnmerge<Tracklet>> ev = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(merge->getNext());
       if (ev == nullptr) {
           /* No Event set, do that now */
           ev = std::shared_ptr<TrackEventUnmerge<Tracklet>>(new TrackEventUnmerge<Tracklet>());
           merge->setNext(ev);
       }
       if (ev->getType() == TrackEvent<Tracklet>::EVENT_TYPE_UNMERGE) {
           ev->setPrev(merge);
           ev->getNext()->append(next);
           return true;
       }
   }
   /* Event_Type was not Unmerge or merge/next could not be found */
   return false;
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
