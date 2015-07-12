#include "auto_tracklets/autotracklet.h"
#include "genealogy.h"
#include "provider/messagerelay.h"
#include "trackeventdead.h"
#include "trackeventdivision.h"
#include "trackeventlost.h"
#include "trackeventmerge.h"
#include "trackeventunmerge.h"
#include "tracklet.h"

#include <QDebug>
namespace CellTracker {

Genealogy::Genealogy(std::shared_ptr<Project> p) :
    tracklets(std::shared_ptr<QHash<int,std::shared_ptr<Tracklet>>>(new QHash<int,std::shared_ptr<Tracklet>>())),
    annotations(std::shared_ptr<QList<std::shared_ptr<Annotation>>>(new QList<std::shared_ptr<Annotation>>())),
    project(p) { }

std::shared_ptr<Tracklet> Genealogy::getTracklet(int id) const
{
    return tracklets->value(id,nullptr);
}

std::shared_ptr<QHash<int, std::shared_ptr<Tracklet> > > Genealogy::getTracklets() const
{
    return tracklets;
}

bool Genealogy::addTracklet(const std::shared_ptr<Tracklet> &value)
{
    if (tracklets->contains(value->getID()))
        return false;
    tracklets->insert(value->getID(),value);
    return true;
}

int Genealogy::removeTracklet(int id)
{
    return tracklets->remove(id);
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
    if (annotated) {
        std::shared_ptr<Annotation> a = std::shared_ptr<Annotation>(new Annotation());
        a->setAnnotationText(annotation);
        a->setAnnotated(annotated);
        this->annotations->append(a);
    }
}

std::shared_ptr<Object> Genealogy::getObject(int trackId, int frameId, uint32_t objId) const
{
    QList<QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> objs = this->getTracklet(trackId)->getObjectsAt(frameId);
    for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> p: objs)
        if (p.second->getId() == objId)
            return p.second;
    return nullptr;
}

std::shared_ptr<Object> Genealogy::getObjectAt(int frameId, int sliceId, uint32_t objId) const
{
    return this->project->getMovie()->getFrame(frameId)->getSlice(sliceId)->getObject(objId);
}

void Genealogy::addObject(int frameId, int trackId, std::shared_ptr<Object> obj)
{
    std::shared_ptr<Frame> f = this->project->getMovie()->getFrame(frameId);
    this->tracklets->value(trackId)->addToContained(f,obj);
}

void Genealogy::removeObject(int frameId, int trackId, uint32_t objId)
{
    this->tracklets->value(trackId)->removeFromContained(frameId, objId);
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
           daughter->setPrev(ev);
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
    std::shared_ptr<Tracklet> t = this->tracklets->value(trackId,nullptr);
    if (t == nullptr)
        return false;
    t->setNext(std::shared_ptr<TrackEventDead<Tracklet>>(new TrackEventDead<Tracklet>()));
    return true;
}

bool Genealogy::setLost(int trackId)
{
    std::shared_ptr<Tracklet> t = this->tracklets->value(trackId,nullptr);
    if (t == nullptr)
        return false;
    t->setNext(std::shared_ptr<TrackEventLost<Tracklet>>(new TrackEventLost<Tracklet>()));
    return true;
}

bool Genealogy::setOpen(int trackId)
{
    std::shared_ptr<Tracklet> t = this->tracklets->value(trackId,nullptr);
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
           merge->setPrev(ev);
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
           next->setPrev(ev);
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

bool Genealogy::addDaughterTrack(std::shared_ptr<Tracklet> mother, std::shared_ptr<Tracklet> daughter)
{
    if (mother && daughter) {
        std::shared_ptr<TrackEventDivision<Tracklet>> ev = std::static_pointer_cast<TrackEventDivision<Tracklet>>(mother->getNext());
        if (ev == nullptr) {
            /* No Event set, do that now */
            ev = std::shared_ptr<TrackEventDivision<Tracklet>>(new TrackEventDivision<Tracklet>());
            mother->setNext(ev);
            daughter->setPrev(ev);
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

bool Genealogy::addDaughterTrack(std::shared_ptr<Tracklet> mother, std::shared_ptr<Object> daughterObj)
{
    std::shared_ptr<Tracklet> daughter;
    if (daughterObj->getTrackId() == UINT32_MAX) {
        daughter = std::shared_ptr<Tracklet>(new Tracklet());
        daughter->addToContained(this->project->getMovie()->getFrame(daughterObj->getFrameId()),daughterObj);
        daughterObj->setTrackId(daughter->getID());
        this->addTracklet(daughter);
    } else {
        daughter = getTracklet(daughterObj->getTrackId());
    }

    if (mother && daughter) {
        std::shared_ptr<TrackEventDivision<Tracklet>> ev = std::static_pointer_cast<TrackEventDivision<Tracklet>>(mother->getNext());
        if (ev == nullptr) {
            /* No Event set, do that now */
            ev = std::shared_ptr<TrackEventDivision<Tracklet>>(new TrackEventDivision<Tracklet>());
            mother->setNext(ev);
            daughter->setPrev(ev);
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

bool Genealogy::setDead(std::shared_ptr<Tracklet> t)
{
    if (t == nullptr)
        return false;
    t->setNext(std::shared_ptr<TrackEventDead<Tracklet>>(new TrackEventDead<Tracklet>()));
    return true;
}

bool Genealogy::setLost(std::shared_ptr<Tracklet> t)
{
    if (t == nullptr)
        return false;
    t->setNext(std::shared_ptr<TrackEventLost<Tracklet>>(new TrackEventLost<Tracklet>()));
    return true;
}

bool Genealogy::setOpen(std::shared_ptr<Tracklet> t)
{
    if (t == nullptr)
        return false;
    t->setNext(nullptr);
    return true;
}

bool Genealogy::addMerge(std::shared_ptr<Tracklet> prev, std::shared_ptr<Tracklet> merge)
{
    if (prev && merge) {
        std::shared_ptr<TrackEventMerge<Tracklet>> ev = std::static_pointer_cast<TrackEventMerge<Tracklet>>(prev->getNext());
        if (ev == nullptr) {
            /* No Event set, do that now */
            ev = std::shared_ptr<TrackEventMerge<Tracklet>>(new TrackEventMerge<Tracklet>());
            prev->setNext(ev);
            merge->setPrev(ev);
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

bool Genealogy::addUnmerge(std::shared_ptr<Tracklet> merge, std::shared_ptr<Tracklet> next)
{
    if (merge && next) {
        std::shared_ptr<TrackEventUnmerge<Tracklet>> ev = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(merge->getNext());
        if (ev == nullptr) {
            /* No Event set, do that now */
            ev = std::shared_ptr<TrackEventUnmerge<Tracklet>>(new TrackEventUnmerge<Tracklet>());
            merge->setNext(ev);
            next->setPrev(ev);
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

bool Genealogy::connectObjects(std::shared_ptr<Object> first, std::shared_ptr<Object> second) {
    if(!first || !second) {
        MessageRelay::emitUpdateStatusBar(QString("Either the first or the second object was a nullptr. (line %1)")
                                          .arg(__LINE__));
        return false;
    }

    /* If the objects are the same and are not yet associated to any tracklet (thus the object only appears in some auto_tracklet).  */
    if(first==second && !first->isInTracklet()) {
        /* Create a new tracklet and add object to it */
        std::shared_ptr<Tracklet> t = std::shared_ptr<Tracklet>(new Tracklet());
        std::shared_ptr<Frame> f = this->project->getMovie()->getFrame(first->getFrameId());

        if(!t || !f) {
            MessageRelay::emitUpdateStatusBar(QString("Either no frame for the first object could be found or you are out of memory (line %1)")
                                              .arg(__LINE__));
            return false;
        }

        if(t && f) {
            t->addToContained(f, first);
            this->addTracklet(t);
            MessageRelay::emitUpdateStatusBar(QString("Added object %1 to a new tracklet %2")
                                              .arg(first->getId())
                                              .arg(t->getID()));
            return true;
        }
    }

    /* First check if "first" appears in a frame prior to "second" -> What do we do if it's the other way around? */
    if(first->getFrameId() < second->getFrameId()) {
        /* If both objects are not associated with any tracklet */
        if(!first->isInTracklet() && !second->isInTracklet()) {
            /* If both objects belong to the same auto_tracklet */
            if(first->getAutoId() == first->getAutoId()) {
                /* Create new tracklet and add all objects from first to second to it */
                std::shared_ptr<Tracklet> t = std::shared_ptr<Tracklet>(new Tracklet());
                std::shared_ptr<AutoTracklet> at =  this->project->getAutoTracklet(first->getAutoId());
                if (!t || !at) {
                    MessageRelay::emitUpdateStatusBar(QString("Either no autotracklet for tracklet %1 could be found or you are out of memory (line %2)")
                                                      .arg(first->getAutoId())
                                                      .arg(__LINE__));
                    return false;
                }

                for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> pair: at->getComponents())
                    if (pair.first->getID() >= first->getFrameId() && pair.first->getID() <= second->getFrameId())
                        t->addToContained(pair);
                this->addTracklet(t);

                MessageRelay::emitUpdateStatusBar(QString("Created new tracklet %1 and added all objects from tracklet %2 to it")
                                                  .arg(t->getID())
                                                  .arg(first->getId()));

                return true;
            }
        }

        /* If the first object belongs to an tracklet */
        else if(first->isInTracklet() && !second->isInTracklet()) {
            /* If the 'second' object appears one frame after the object 'first' */
            if(first->getFrameId() == second->getFrameId()-1) {
                /* Add 'second' to the tracklet of 'first' */
                std::shared_ptr<Tracklet> t = getTracklet(first->getTrackId());
                std::shared_ptr<Frame> f = this->project->getMovie()->getFrame(second->getFrameId());

                if(!t || !f) {
                    MessageRelay::emitUpdateStatusBar(QString("Either tracklet %1 of object %2 or frame %3 could not be fonud (line %4)")
                                                      .arg(first->getTrackId())
                                                      .arg(first->getId())
                                                      .arg(second->getFrameId())
                                                      .arg(__LINE__));
                    return false;
                }

                t->addToContained(f, second);
                MessageRelay::emitUpdateStatusBar(QString("Adding object %1 to tracklet %2")
                                                  .arg(second->getId())
                                                  .arg(t->getID()));

                return true;
            } else {
                /* What to do here? Do we add all objects between 'first.frameID' and 'second.frameID', which
                     * belong to the auto_tracklet of 'second', to the tracklet of 'first'? Or do we just add second
                     * to the tracklet of 'first'? */
                MessageRelay::emitUpdateStatusBar(QString("This case is unimplemented (line %1)")
                                                  .arg(__LINE__));
                return false;
            }
        }
        /* Is that possible, that the 'first' objects belongs to an auto_tracklet and the 'second' don't?
             * Isn't then the first condition "first->getFrameID() <= second->getFrameID()" violated? */
        else if(!first->isInTracklet() && second->isInTracklet()) {
            // emit("error?");
            MessageRelay::emitUpdateStatusBar(QString("This case is unimplemented (line %1)")
                                              .arg(__LINE__));
            return false;
        }
        /* Both belong to an tracklet */
        else {
            /* If both belong to different tracklets */
            if(first->getTrackId() != second->getTrackId()) {
                /* If 'first' is the last object (end) of tracklet i and second is the first object (start) of
                 * tracklet j, Then connect the join tracklets. */
                std::shared_ptr<Tracklet> firstTracklet = getTracklet(first->getTrackId());
                std::shared_ptr<Tracklet> secondTracklet = getTracklet(second->getTrackId());
                if(!firstTracklet || !secondTracklet) {
                    MessageRelay::emitUpdateStatusBar(QString("Either tracklet %1 of object %2 or tracklet %3 of object %4 could not be found")
                                                      .arg(first->getTrackId())
                                                      .arg(first->getId())
                                                      .arg(second->getTrackId())
                                                      .arg(second->getId()));
                    return false;
                }

                if(first == firstTracklet->getEnd().second && second == secondTracklet->getStart().second) {
                    // joinTracklets(first->getTracklet(), second->getTracklet());
                    for (auto p: secondTracklet->getContained())
                        firstTracklet->addToContained(p);

                    firstTracklet->setNext(secondTracklet->getNext());

                    MessageRelay::emitUpdateStatusBar(QString("Joined tracklets %1 and %2")
                                                      .arg(firstTracklet->getID())
                                                      .arg(secondTracklet->getID()));

                    this->removeTracklet(secondTracklet->getID());
                    return true;
                }
                /* If 'first' is the last object (end) of tracklet i and second is NOT the first object (start) of tracklet j, Then?? */
                else if(first == firstTracklet->getEnd().second && second != secondTracklet->getStart().second) {
                    /* Not yet decided how to solve this situation */
                    MessageRelay::emitUpdateStatusBar(QString("This case is unimplemented"));
                    return false;
                }
                /* If 'first' is NOT the last object (end) of tracklet i and second is the first object (start) of tracklet j, Then?? */
                else if(first != firstTracklet->getEnd().second && second==secondTracklet->getStart().second) {
                    /* Not yet decided how to solve this situation */
                    MessageRelay::emitUpdateStatusBar(QString("This case is unimplemented"));
                }
            }
            /* Else: 'first' and 'second' already belong to the same tracklet */
        }
    }
    MessageRelay::emitUpdateStatusBar(QString("No suitable case found"));
    return false;
}

void Genealogy::allFromAT(std::shared_ptr<Tracklet> t, std::shared_ptr<AutoTracklet> at)
{
    if(!t || !at)
        return;
    for (QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>> p: at->getComponents())
        t->addToContained(p);
}

void Genealogy::allFromATBetween(std::shared_ptr<Tracklet> t,
                                           std::shared_ptr<AutoTracklet> at,
                                           std::shared_ptr<Frame> from,
                                           std::shared_ptr<Frame> to)
{
    if(!t || !at || !from || !to)
        return;
    for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> p: at->getComponents())
        if (p.first->getID() >= from->getID() && p.first->getID() <= to->getID())
            t->addToContained(p);
}

void Genealogy::allFromATFrom(std::shared_ptr<Tracklet> t,
                                        std::shared_ptr<AutoTracklet> at,
                                        std::shared_ptr<Frame> from)
{
    if(!t || !at || !from)
        return;
    for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> p: at->getComponents())
        if (p.first->getID() >= from->getID())
            t->addToContained(p);
}

void Genealogy::allFromATUntil(std::shared_ptr<Tracklet> t,
                                      std::shared_ptr<AutoTracklet> at,
                                      std::shared_ptr<Frame> to)
{
    if(!t || !at || !to)
        return;
    for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> p: at->getComponents())
        if (p.first->getID() <= to->getID())
            t->addToContained(p);
}


}

std::ostream &operator<<(std::ostream &strm, CellTracker::Genealogy &g)
{
    strm << "Genealogy:" << std::endl;
    strm << "  annotations:" << std::endl;
    for (std::shared_ptr<CellTracker::Annotation> a: *(g.annotations))
        strm << "    " << *a;
    strm << "  tracklets:" << std::endl;
    for (std::shared_ptr<CellTracker::Tracklet> t: *(g.tracklets))
        strm << "    " << *t;
    return strm;
}
