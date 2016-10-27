#include "base/autotracklet.h"
#include "genealogy.h"
#include "provider/messagerelay.h"
#include "trackeventdead.h"
#include "trackeventdivision.h"
#include "trackeventlost.h"
#include "trackeventmerge.h"
#include "trackeventunmerge.h"
#include "trackeventendofmovie.h"
#include "tracklet.h"

#include <QDebug>
namespace CellTracker {

/*!
 * \brief creates a new Genealogy for a Project
 * \param p the Project of this Genealogy
 */
Genealogy::Genealogy(std::shared_ptr<Project> p) :
    tracklets(new QHash<int,std::shared_ptr<Tracklet>>()),
    annotations(new QList<std::shared_ptr<Annotation>>()),
    annotated(new QList<std::shared_ptr<Annotateable>>()),
    project(p) {}

/*!
 * \brief gets an Annotation
 * \param id the ID of the Annotation to return
 * \return the Annotation or nullptr
 */
std::shared_ptr<Annotation> Genealogy::getAnnotation(int id) const
{
    for (std::shared_ptr<Annotation> a : *annotations)
        if (id >= 0 && a->getId() == (uint32_t)id)
            return a;
    return nullptr;
}

/*!
 * \brief gets a Tracklet
 * \param id the ID of the Tracklet to return
 * \return the Tracklet or nullptr
 */
std::shared_ptr<Tracklet> Genealogy::getTracklet(int id) const
{
    return tracklets->value(id,nullptr);
}

/*!
 * \brief returns all Tracklet%s
 * \return the Tracklets in a QHash
 */
std::shared_ptr<QHash<int, std::shared_ptr<Tracklet> > > Genealogy::getTracklets() const
{
    return tracklets;
}

/*!
 * \brief adds a Tracklet to the Genealogy
 * \param value the Tracklet to add
 * \return true if successful, false if already in the Genealogy
 */
bool Genealogy::addTracklet(const std::shared_ptr<Tracklet> &value)
{
    if (tracklets->contains(value->getId()))
        return false;
    tracklets->insert(value->getId(),value);
    return true;
}

/*!
 * \brief removes a Tracklet from the Genealogy
 * \param id the ID of the Tracklet to remove
 * \return the number of removed Elements (should be 1 or 0)
 */
int Genealogy::removeTracklet(int id)
{
    std::weak_ptr<Tracklet> t = tracklets->value(id);
    std::shared_ptr<TrackEvent<Tracklet>> next = t.lock()->getNext();
    std::shared_ptr<TrackEvent<Tracklet>> prev = t.lock()->getPrev();

    /* clear references to this tracklet */
    if (prev) {
        switch (prev->getType()) {
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_DEAD:
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_ENDOFMOVIE:
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_LOST:
            qDebug() << "TrackEvent{Dead,EndOfMovie,Lost} should never be set as previous";
            break;
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_DIVISION: {
            /* one previous */
            std::shared_ptr<TrackEventDivision<Tracklet>> ted = std::static_pointer_cast<TrackEventDivision<Tracklet>>(prev);
            std::shared_ptr<QList<std::weak_ptr<Tracklet>>> siblings = ted->getNext();
            QMutableListIterator<std::weak_ptr<Tracklet>> it(*siblings);
            while (it.hasNext())
                if (it.next().lock() == t.lock())
                    it.remove();
            /* if this was the last next tracklet, remove the reference to the trackevent from the previous tracklet */
            if (siblings->isEmpty() && ted->getPrev().lock()) {
                ted->getPrev().lock()->setNext(nullptr);
                ted->setPrev(std::weak_ptr<Tracklet>());
            }
            break; }
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_UNMERGE: {
            /* one previous */
            std::shared_ptr<TrackEventUnmerge<Tracklet>> teu = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(prev);
            std::shared_ptr<QList<std::weak_ptr<Tracklet>>> siblings = teu->getNext();
            QMutableListIterator<std::weak_ptr<Tracklet>> it(*siblings);
            while (it.hasNext())
                if (it.next().lock() == t.lock())
                    it.remove();
            /* if this was the last next tracklet, remove the reference to the trackevent from the previous tracklet */
            if (siblings->isEmpty() && teu->getPrev().lock()) {
                teu->getPrev().lock()->setNext(nullptr);
                teu->setPrev(std::weak_ptr<Tracklet>());
            }
            break; }
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_MERGE: {
            /* multiple previous */
            std::shared_ptr<TrackEventMerge<Tracklet>> tem = std::static_pointer_cast<TrackEventMerge<Tracklet>>(prev);
            std::shared_ptr<QList<std::weak_ptr<Tracklet>>> ancestors = tem->getPrev();
            /* remove references to the TrackEvent from all previous tracklets */
            for (std::weak_ptr<Tracklet> ancestor : *ancestors) {
                ancestor.lock()->setNext(nullptr);
            }
            ancestors->clear();
            if (tem->getNext().lock() == t.lock())
                tem->setNext(std::weak_ptr<Tracklet>());
            break; }
        }
        t.lock()->setPrev(nullptr);
    }
    if (next) {
        switch (next->getType()) {
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_DEAD: {
            std::shared_ptr<TrackEventDead<Tracklet>> ted = std::static_pointer_cast<TrackEventDead<Tracklet>>(next);
            if (ted->getPrev().lock() == t.lock())
                ted->setPrev(std::weak_ptr<Tracklet>());
            break; }
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_ENDOFMOVIE: {
            std::shared_ptr<TrackEventEndOfMovie<Tracklet>> teeom = std::static_pointer_cast<TrackEventEndOfMovie<Tracklet>>(next);
            if (teeom->getPrev().lock() == t.lock())
                teeom->setPrev(std::weak_ptr<Tracklet>());
            break; }
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_LOST: {
            std::shared_ptr<TrackEventLost<Tracklet>> tel = std::static_pointer_cast<TrackEventLost<Tracklet>>(next);
            if (tel->getPrev().lock() == t.lock())
                tel->setPrev(std::weak_ptr<Tracklet>());
            break; }
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_DIVISION: {
            std::shared_ptr<TrackEventDivision<Tracklet>> ted = std::static_pointer_cast<TrackEventDivision<Tracklet>>(next);
            std::shared_ptr<QList<std::weak_ptr<Tracklet>>> daughters = ted->getNext();
            if (ted->getPrev().lock() != t.lock())
                break;
            QMutableListIterator<std::weak_ptr<Tracklet>> it(*daughters);
            for (std::weak_ptr<Tracklet> daughter : *daughters) {
                daughter.lock()->setPrev(nullptr);
            }
            daughters->clear();
            ted->setPrev(std::weak_ptr<Tracklet>());
            break; }
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_UNMERGE: {
            std::shared_ptr<TrackEventUnmerge<Tracklet>> teu = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(next);
            std::shared_ptr<QList<std::weak_ptr<Tracklet>>> daughters = teu->getNext();
            if (teu->getPrev().lock() != t.lock())
                break;
            for (std::weak_ptr<Tracklet> daughter : *daughters) {
                daughter.lock()->setPrev(nullptr);
            }
            daughters->clear();
            teu->setPrev(std::weak_ptr<Tracklet>());
            break; }
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_MERGE: {
            std::shared_ptr<TrackEventMerge<Tracklet>> tem = std::static_pointer_cast<TrackEventMerge<Tracklet>>(next);
            std::shared_ptr<QList<std::weak_ptr<Tracklet>>> siblings = tem->getPrev();
            QMutableListIterator<std::weak_ptr<Tracklet>> it(*siblings);
            while (it.hasNext())
                if (it.next().lock() == t.lock())
                    it.remove();
            if (siblings->isEmpty() && tem->getNext().lock()) {
                tem->getNext().lock()->setPrev(nullptr);
                tem->setNext(std::weak_ptr<Tracklet>());
            }
            break; }
        }
        t.lock()->setNext(nullptr);
    }
    return tracklets->remove(id);
}

/*!
 * \brief returns all Annotation%s
 * \return the Annotations in a QList
 */
std::shared_ptr<QList<std::shared_ptr<Annotation> > > Genealogy::getAnnotations() const
{
    return annotations;
}

/*!
 * \brief sets the Annotation%s
 * \param value the QList of Annotation%s to set
 */
void Genealogy::setAnnotations(const std::shared_ptr<QList<std::shared_ptr<Annotation>>> &value)
{
    annotations = value;
}

/*!
 * \brief adds an Annotation
 * \param a the Annotation to add
 */
void Genealogy::addAnnotation(std::shared_ptr<Annotation> a)
{
    annotations->append(a);
}

/*!
 * \brief removes an Annotation
 * \param a the Annotation to remove
 *
 * Also removes all references from the QList of Annotateables that point to
 * this Annotation
 */
void Genealogy::deleteAnnotation(std::shared_ptr<Annotation> a)
{
    /* remove from annotations */
    annotations->removeOne(a);

    /* remove references from annotated */
    for (std::shared_ptr<Annotateable> abl : *annotated) {
        if (abl->getAnnotations()->contains(a))
            abl->unannotate(a);
    }
}

/*!
 * \brief annotates a given Annotateable with an Annotation
 * \param annotatee the Annotateable to annotate
 * \param annotation the Annotation to add to this Annotateable
 */
void Genealogy::annotate(std::shared_ptr<Annotateable> annotatee, std::shared_ptr<Annotation> annotation)
{
    if (!annotatee || !annotation)
        return;
    if (annotations->contains(annotation)) {
        annotatee->annotate(annotation);
        if (!annotated->contains(annotatee))
            annotated->append(annotatee);
    }
}

/*!
 * \brief unannotates a given Annotation from an Annotateable
 * \param annotatee the Annotateable to unannotate
 * \param annotation the Annotation to remove from this Annotateable
 *
 * Also removes the Annotateable from the list of Annotated Objects if it
 * contains no more Annotations.
 */
void Genealogy::unannotate(std::shared_ptr<Annotateable> annotatee, std::shared_ptr<Annotation> annotation)
{
    if (!annotatee || !annotation)
        return;
    if (annotations->contains(annotation)) {
        annotatee->unannotate(annotation);
        if (!annotatee->isAnnotated())
            annotated->removeOne(annotatee);
    }

}

/*!
 * \brief returns an Object by its Track-/Frame- and ObjectID
 * \param trackId the TrackID of this Object
 * \param frameId the FrameID of this Object
 * \param objId the ObjectID of this Object
 * \return the Object or nullptr if it wasn't found
 */
std::shared_ptr<Object> Genealogy::getObject(int trackId, int frameId, uint32_t objId) const
{
    std::shared_ptr<Tracklet> t = this->getTracklet(trackId);
    if (!t) return nullptr;
    QList<QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> objs = t->getObjectsAt(frameId);
    for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> p: objs)
        if (p.second->getId() == objId)
            return p.second;
    return nullptr;
}

/*!
 * \brief returns an Object by its Frame-/Slice-/Channel- and ObjectID
 * \param frameId the FrameID of this Object
 * \param sliceId the SliceID of this Object
 * \param chanId the ChannelID of this Object
 * \param objId the ObjectID of this Object
 * \return the Object or nullptr if it wasn't found
 */
std::shared_ptr<Object> Genealogy::getObjectAt(int frameId, int sliceId, int chanId, uint32_t objId) const
{
    std::shared_ptr<Movie> m = this->project.lock()->getMovie();
    if (!m) return nullptr;
    std::shared_ptr<Frame> f = m->getFrame(frameId);
    if (!f) return nullptr;
    std::shared_ptr<Slice> s = f->getSlice(sliceId);
    if (!s) return nullptr;
    std::shared_ptr<Channel> c = s->getChannel(chanId);
    if (!c) return nullptr;
    std::shared_ptr<Object> o = c->getObject(objId);
    return o;
}

/*!
 * \brief adds an Object to a Tracklet at a given FrameID
 * \param frameId the FrameID of the Object
 * \param trackId the TrackID of the Track to add to
 * \param obj the Object to add
 */
void Genealogy::addObject(int frameId, int trackId, std::shared_ptr<Object> const &obj)
{
    std::shared_ptr<Frame> f = this->project.lock()->getMovie()->getFrame(frameId);
    this->tracklets->value(trackId)->addToContained(f,obj);
}

/*!
 * \brief removes an Object from a tracklet
 * \param frameId the FrameID of the Object
 * \param trackId the TrackID of the Track to remove from
 * \param objId the ObjectID of the Object to remove
 */
void Genealogy::removeObject(int frameId, int trackId, uint32_t objId)
{
    this->tracklets->value(trackId)->removeFromContained(frameId, objId);
}

/*!
 * \brief adds a daughter Tracklet to a given Tracklet
 * \param mother the mother Tracklet
 * \param daughterObj the daughter Object
 * \return true if it succeeded
 *
 * This adds a daughter Tracklet to a given mother Tracklet. If the
 * daughterObject is already in a Tracklet, this Tracklet is added as the
 * daughter Tracklet, if not, a new one is created.
 */
bool Genealogy::addDaughterTrack(std::shared_ptr<Tracklet> mother, std::shared_ptr<Object> daughterObj)
{
    std::shared_ptr<Tracklet> daughter;

    if (!mother || !daughterObj) /* Function was called falsely */
        return false;
    if (daughterObj->getFrameId() < mother->getStart().first->getID()) /* daughter Frame is prior to begin of tracklet */
        return false;

    if (daughterObj->getTrackId() == UINT32_MAX) {
        daughter = std::make_shared<Tracklet>();
        daughter->addToContained(this->project.lock()->getMovie()->getFrame(daughterObj->getFrameId()),daughterObj);
        daughterObj->setTrackId(daughter->getId());
        this->addTracklet(daughter);
    } else {
        daughter = getTracklet(daughterObj->getTrackId());
    }

    if (mother && daughter) {
        std::shared_ptr<TrackEventDivision<Tracklet>> ev = std::static_pointer_cast<TrackEventDivision<Tracklet>>(mother->getNext());
        if (ev == nullptr) {
            /* No Event set, do that now */
            ev = std::make_shared<TrackEventDivision<Tracklet>>();
            mother->setNext(ev);
        }
        if (ev->getType() == TrackEvent<Tracklet>::EVENT_TYPE_DIVISION) {
            ev->setPrev(mother);
            bool contained = false;
            for (std::weak_ptr<Tracklet> t : *ev->getNext()) {
                if (t.lock() == daughter) {
                    contained = true;
                    break;
                }
            }
            if (!contained) {
                ev->getNext()->append(daughter);
                daughter->setPrev(ev);
            }
            return true;
        }
    }
    /* Event_Type was not Divsion or mother/daughter could not be found*/
    return false;
}

bool Genealogy::addUnmergedTrack(std::shared_ptr<Tracklet> merged, std::shared_ptr<Object> unmergedObj)
{
    std::shared_ptr<Tracklet> unmerged;

    if (!merged || !unmergedObj) /* Function was called falsely */
        return false;
    if (unmergedObj->getFrameId() < merged->getStart().first->getID()) /* merged Frame is prior to begin of tracklet */
        return false;

    if (unmergedObj->getTrackId() == UINT32_MAX) {
        unmerged = std::make_shared<Tracklet>();
        unmerged->addToContained(this->project.lock()->getMovie()->getFrame(unmergedObj->getFrameId()),unmergedObj);
        unmergedObj->setTrackId(unmerged->getId());
        this->addTracklet(unmerged);
    } else {
        unmerged = getTracklet(unmergedObj->getTrackId());
    }

    if (merged && unmerged) {
        std::shared_ptr<TrackEventUnmerge<Tracklet>> ev = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(merged->getNext());
        if (ev == nullptr) {
            /* No Event set, do that now */
            ev = std::make_shared<TrackEventUnmerge<Tracklet>>();
            merged->setNext(ev);
        }
        if (ev->getType() == TrackEvent<Tracklet>::EVENT_TYPE_UNMERGE) {
            ev->setPrev(merged);
            ev->getNext()->append(unmerged);
            unmerged->setPrev(ev);
            return true;
        }
    }
    /* Event_Type was not Unmerge or merged/unmerged could not be found*/
    return false;
}

bool Genealogy::addMergedTrack(std::shared_ptr<Tracklet> unmerged, std::shared_ptr<Object> mergedObj)
{
    std::shared_ptr<Tracklet> merged;

    if (!unmerged || !mergedObj) /* Function was called falsely */
        return false;
    if (mergedObj->getFrameId() < unmerged->getStart().first->getID()) /* unmerged Frame is prior to begin of tracklet */
        return false;

    if (mergedObj->getTrackId() == UINT32_MAX) {
        merged = std::make_shared<Tracklet>();
        merged->addToContained(this->project.lock()->getMovie()->getFrame(mergedObj->getFrameId()),mergedObj);
        mergedObj->setTrackId(merged->getId());
        this->addTracklet(merged);
    } else {
        merged = getTracklet(mergedObj->getTrackId());
    }

    if (merged && unmerged) {
        std::shared_ptr<TrackEventMerge<Tracklet>> ev = std::static_pointer_cast<TrackEventMerge<Tracklet>>(merged->getPrev());
        if (ev == nullptr) {
            /* No Event set, do that now */
            ev = std::make_shared<TrackEventMerge<Tracklet>>();
            merged->setPrev(ev);
        }
        if (ev->getType() == TrackEvent<Tracklet>::EVENT_TYPE_MERGE) {
            bool contained = false;
            for (std::weak_ptr<Tracklet> t : *ev->getPrev()) {
                if (t.lock() == unmerged) {
                    contained = true;
                    break;
                }
            }
            if (!contained)
                ev->getPrev()->append(unmerged);
            ev->setNext(merged);
            unmerged->setNext(ev);
            return true;
        }
    }
    /* Event_Type was not Merge or merged/unmerged could not be found*/
    return false;
}

bool Genealogy::hasDaughterObject(std::shared_ptr<Tracklet> mother, std::shared_ptr<Object> daughterObj)
{
    if (!mother || !mother->getNext() || mother->getNext()->getType() !=
            TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_DIVISION)
        return false;
    std::shared_ptr<TrackEventDivision<Tracklet>> ted =
            std::static_pointer_cast<TrackEventDivision<Tracklet>>(mother->getNext());
    for (std::weak_ptr<Tracklet> t : *ted->getNext())
        for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> p : t.lock()->getContained().values())
            if (p.second->getId() == daughterObj->getId() && p.second->getFrameId() == daughterObj->getFrameId())
                return true;
    return false;
}

bool Genealogy::hasUnmergerObject(std::shared_ptr<Tracklet> merged, std::shared_ptr<Object> unmergedObj)
{
    if (!merged || !merged->getNext() || merged->getNext()->getType() !=
            TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_UNMERGE)
        return false;
    std::shared_ptr<TrackEventUnmerge<Tracklet>> teu =
            std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(merged->getNext());
    for (std::weak_ptr<Tracklet> t : *teu->getNext())
        for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> p : t.lock()->getContained().values())
            if (p.second->getId() == unmergedObj->getId() && p.second->getFrameId() == unmergedObj->getFrameId())
                return true;
    return false;
}

bool Genealogy::hasMergerObject(std::shared_ptr<Tracklet> unmerged, std::shared_ptr<Object> mergedObj)
{
    if (!unmerged || !unmerged->getPrev() || unmerged->getPrev()->getType() !=
            TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_MERGE)
        return false;
    std::shared_ptr<TrackEventMerge<Tracklet>> tem =
            std::static_pointer_cast<TrackEventMerge<Tracklet>>(unmerged->getPrev());
    for (std::weak_ptr<Tracklet> t : *tem->getPrev())
        for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> p : t.lock()->getContained().values())
            if (p.second->getId() == mergedObj->getId() && p.second->getFrameId() == mergedObj->getFrameId())
                return true;
    return false;
}

bool Genealogy::removeDaughterTrack(std::shared_ptr<Tracklet> mother, std::shared_ptr<Object> daughterObj)
{
    if (!mother || !daughterObj || !mother->getNext() || mother->getNext()->getType() !=
            TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_DIVISION)
        return false;
    std::shared_ptr<TrackEventDivision<Tracklet>> ted =
            std::static_pointer_cast<TrackEventDivision<Tracklet>>(mother->getNext());
    std::shared_ptr<QList<std::weak_ptr<Tracklet>>> next = ted->getNext();
    std::shared_ptr<Tracklet> nextT;
    for (std::weak_ptr<Tracklet> t : *next) {
        std::shared_ptr<Tracklet> sp = t.lock();
        for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> p : t.lock()->getContained().values()) {
            if (p.second->getId() == daughterObj->getId() && p.second->getFrameId() == daughterObj->getFrameId()) {
                if (sp->getContained().count() == 1)
                    nextT = sp;
                else
                    return false;
            }
        }
    }

    QMutableListIterator<std::weak_ptr<Tracklet>> it(*ted->getNext());
    while (it.hasNext())
        if (it.next().lock() == nextT)
            it.remove();

    removeTracklet(nextT->getId());
    /*! \todo this should be done somewhere else */
    daughterObj->setTrackId(UINT32_MAX);
    nextT->setPrev(nullptr);
    if (ted->getNext()->count() == 0)
        mother->setNext(nullptr);

    return true;
}

bool Genealogy::removeUnmergedTrack(std::shared_ptr<Tracklet> merged, std::shared_ptr<Object> unmergedObj)
{
    if (!merged || !unmergedObj || !merged->getNext() || merged->getNext()->getType() !=
            TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_UNMERGE)
        return false;
    std::shared_ptr<TrackEventUnmerge<Tracklet>> ted =
            std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(merged->getNext());
    std::shared_ptr<QList<std::weak_ptr<Tracklet>>> next = ted->getNext();
    std::shared_ptr<Tracklet> nextT;
    for (std::weak_ptr<Tracklet> t : *next) {
        std::shared_ptr<Tracklet> sp = t.lock();
        for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> p : t.lock()->getContained().values()) {
            if (p.second->getId() == unmergedObj->getId() && p.second->getFrameId() == unmergedObj->getFrameId()) {
                if (sp->getContained().count() == 1)
                    nextT = sp;
                else
                    return false;
            }
        }
    }

    QMutableListIterator<std::weak_ptr<Tracklet>> it(*ted->getNext());
    while (it.hasNext())
        if (it.next().lock() == nextT)
            it.remove();

    removeTracklet(nextT->getId());
    /*! \todo this should be done somewhere else */
    unmergedObj->setTrackId(UINT32_MAX);
    nextT->setPrev(nullptr);
    if (ted->getNext()->count() == 0)
        merged->setNext(nullptr);

    return true;
}

bool Genealogy::removeMergedTrack(std::shared_ptr<Tracklet> unmerged, std::shared_ptr<Object> mergedObj)
{
    if (!unmerged || !mergedObj || !unmerged->getPrev() || unmerged->getPrev()->getType() !=
            TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_MERGE)
        return false;
    std::shared_ptr<TrackEventMerge<Tracklet>> ted =
            std::static_pointer_cast<TrackEventMerge<Tracklet>>(unmerged->getPrev());
    std::shared_ptr<QList<std::weak_ptr<Tracklet>>> prev = ted->getPrev();
    std::shared_ptr<Tracklet> prevT;
    for (std::weak_ptr<Tracklet> t : *prev) {
        std::shared_ptr<Tracklet> sp = t.lock();
        for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> p : t.lock()->getContained().values()) {
            if (p.second->getId() == mergedObj->getId() && p.second->getFrameId() == mergedObj->getFrameId()) {
                if (sp->getContained().count() == 1)
                    prevT = sp;
                else
                    return false;
            }
        }
    }

    QMutableListIterator<std::weak_ptr<Tracklet>> it(*ted->getPrev());
    while (it.hasNext())
        if (it.next().lock() == prevT)
            it.remove();

    removeTracklet(prevT->getId());
    mergedObj->setTrackId(UINT32_MAX);
    prevT->setNext(nullptr);
    if (ted->getPrev()->count() == 0)
        unmerged->setPrev(nullptr);

    return true;
}

/*!
 * \brief sets a given Tracklet to Status Dead
 * \param t the Tracklet to set dead
 * \return true if it succeeded
 */
bool Genealogy::setDead(std::shared_ptr<Tracklet> t)
{
    if (t == nullptr)
        return false;
    t->setNext(std::make_shared<TrackEventDead<Tracklet>>());
    return true;
}

/*!
 * \brief sets a given Tracklet to Status Lost
 * \param t the Tracklet to set lost
 * \return true if it succeeded
 */
bool Genealogy::setLost(std::shared_ptr<Tracklet> t)
{
    if (t == nullptr)
        return false;
    t->setNext(std::make_shared<TrackEventLost<Tracklet>>());
    return true;
}

/*!
 * \brief sets a given Tracklet to Status Open
 * \param t the Tracklet to set open
 * \return true if it succeeded
 */
bool Genealogy::setOpen(std::shared_ptr<Tracklet> t)
{
    if (t == nullptr)
        return false;
    t->setNext(nullptr);
    return true;
}

/*!
 * \brief adds a previous Tracklet to a TrackEventMerge
 * \param prev the previous Tracklet
 * \param merge the merged Tracklet
 * \return true if it succeeded
 */
bool Genealogy::addMerge(std::shared_ptr<Tracklet> prev, std::shared_ptr<Tracklet> merge)
{
    if (prev && merge) {
        std::shared_ptr<TrackEventMerge<Tracklet>> ev = std::static_pointer_cast<TrackEventMerge<Tracklet>>(prev->getNext());
        if (ev == nullptr) {
            /* No Event set, do that now */
            ev = std::make_shared<TrackEventMerge<Tracklet>>();
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

/*!
 * \brief adds a next Tracklet to a TrackEventUnmerge
 * \param merge the merge Tracklet
 * \param next the next Tracklet
 * \return true if it succeeded
 */
bool Genealogy::addUnmerge(std::shared_ptr<Tracklet> merge, std::shared_ptr<Tracklet> next)
{
    if (merge && next) {
        std::shared_ptr<TrackEventUnmerge<Tracklet>> ev = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(merge->getNext());
        if (ev == nullptr) {
            /* No Event set, do that now */
            ev = std::make_shared<TrackEventUnmerge<Tracklet>>();
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

/*!
 * \brief returns all annotated Annotateable%s
 * \return a QList of Annotateable Objects
 */
std::shared_ptr<QList<std::shared_ptr<Annotateable> > > Genealogy::getAnnotated() const
{
    return annotated;
}

/*!
 * \brief sets the annotated Annotateable%s
 * \param value the QList of Annotateable%s to set
 */
void Genealogy::setAnnotated(const std::shared_ptr<QList<std::shared_ptr<Annotateable> > > &value)
{
    annotated = value;
}

/*!
 * \brief connects two Objects
 * \param first the first Object
 * \param second the second Object
 * \return true on success, false on failure
 *
 * \warning Some cases in this function are unimplemented. If one is encountered,
 * a message in the StatusBar will be printed
 *
 * Pseudo-Code for this function looks like this:
 *
 * \code
 *   if first is the same as second and not in a Tracklet
 *     -> add first to new Tracklet, return true
 *   if first is prior to second?
 *     if both are not in a Tracklet
 *       if both have the same AutoTracklet
 *         -> Create new tracklet and add all objects from first to second to it, return true;
 *     else if only the first is in a tracklet
 *       if the second is directly after first
 *         -> Add 'second' to the tracklet of 'first', return true;
 *       else
 *         -> Add all objects between end of Tracklet of first and the Frame of second to the Tracklet of first, return true
 *     else if first is not in a Tracklet, second is in a Tracklet
 *       -> report error via message, return false
 *     else, both are in a Tracklet
 *       if both in different Tracklets
 *         if first is end of tracklet, second is start of a tracklet?
 *           -> join Tracklets of first and second, return true
 *         else if first is end of tracklet, second is not start of a tracklet
 *           -> unimplemented, return false
 *         else if first is not end of a tracklet, second is start of a tracklet
 *           -> unimplemented, return false
 *   -> no suitable case, return false
 * \endcode
 */
bool Genealogy::connectObjects(std::shared_ptr<Object> first, std::shared_ptr<Object> second) {
    if(!first || !second) {
        MessageRelay::emitUpdateStatusBar(QString("Either the first or the second object was a nullptr. (line %1)")
                                          .arg(__LINE__));
        return false;
    }

    /* If the objects are the same and are not yet associated to any tracklet (thus the object only appears in some auto_tracklet).  */
    if(first==second && !first->isInTracklet()) {
        /* Create a new tracklet and add object to it */
        auto t = std::make_shared<Tracklet>();
        std::shared_ptr<Frame> f = this->project.lock()->getMovie()->getFrame(first->getFrameId());

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
                                              .arg(t->getId()));
            return true;
        }
    }

    /* First check if "first" appears in a frame prior to "second" -> What do we do if it's the other way around? */
    if(first->getFrameId() < second->getFrameId()) {
        /* If both objects are not associated with any tracklet */
        if(!first->isInTracklet() && !second->isInTracklet()) {
            /* If both objects belong to the same auto_tracklet */
            if(first->getAutoId() == second->getAutoId()) {
                /* Create new tracklet and add all objects from first to second to it */
                auto t = std::make_shared<Tracklet>();
                std::shared_ptr<AutoTracklet> at =  this->project.lock()->getAutoTracklet(first->getAutoId());
                if (!t || !at) {
                    MessageRelay::emitUpdateStatusBar(QString("Either no autotracklet for tracklet %1 could be found or you are out of memory (line %2)")
                                                      .arg(first->getAutoId())
                                                      .arg(__LINE__));
                    return false;
                }
                for (auto pair: at->getComponents().toStdMap()) {
                    if (pair.first >= 0 && (uint32_t)pair.first >= first->getFrameId() &&
                            (uint32_t)pair.first <= second->getFrameId()) {
                        std::shared_ptr<Frame> f = this->project.lock()->getMovie()->getFrame(pair.first);
                        QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>> p(f, pair.second);
                        t->addToContained(p);
                    }
                }
                this->addTracklet(t);

                MessageRelay::emitUpdateStatusBar(QString("Created new tracklet %1 and added all objects from tracklet %2 to it")
                                                  .arg(t->getId())
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
                std::shared_ptr<Frame> f = this->project.lock()->getMovie()->getFrame(second->getFrameId());

                if(!t || !f) {
                    MessageRelay::emitUpdateStatusBar(QString("Either tracklet %1 of object %2 or frame %3 could not be fonud (line %4)")
                                                      .arg(first->getTrackId())
                                                      .arg(first->getId())
                                                      .arg(second->getFrameId())
                                                      .arg(__LINE__));
                    return false;
                }

                QList<QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> l = t->getObjectsAt(second->getFrameId());
                for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> p : l)
                    if (p.first->getID() == second->getFrameId())
                        t->removeFromContained(p.first->getID(), p.second->getId());

                t->addToContained(f, second);

                MessageRelay::emitUpdateStatusBar(QString("Adding object %1 to tracklet %2")
                                                  .arg(second->getId())
                                                  .arg(t->getId()));

                return true;
            } else {
                /* What to do here? Do we add all objects between 'first.frameID' and 'second.frameID', which
                     * belong to the auto_tracklet of 'second', to the tracklet of 'first'? Or do we just add second
                     * to the tracklet of 'first'? */
                std::shared_ptr<Tracklet> t = getTracklet(first->getTrackId());
                std::shared_ptr<AutoTracklet> at = this->project.lock()->getAutoTracklet(second->getAutoId());
                std::shared_ptr<Frame> f = this->project.lock()->getMovie()->getFrame(second->getFrameId());

                if(!t || !f ) {
                    MessageRelay::emitUpdateStatusBar(QString("Either tracklet %1 of object %2 or frame %3 could not be found (line %4)")
                                                      .arg(first->getTrackId())
                                                      .arg(first->getId())
                                                      .arg(second->getFrameId())
                                                      .arg(__LINE__));
                    return false;
                }

                int trackletEnd = t->getEnd().first->getID();

                if (at) {
                    for (int atFrame : at->getComponents().keys()) {
                        if (atFrame > trackletEnd && atFrame >= 0 && (uint32_t)atFrame <= f->getID()) {
                            std::shared_ptr<Frame> newFrame = this->project.lock()->getMovie()->getFrame(atFrame);
                            std::shared_ptr<Object> newObject = at->getComponents().value(atFrame);
                            t->addToContained(newFrame, newObject);
                        }
                    }
                    MessageRelay::emitUpdateStatusBar(QString("Added all objects between %1 and %2 in AutoTracklet %3 to Tracklet %4")
                                                      .arg(trackletEnd)
                                                      .arg(f->getID())
                                                      .arg(at->getID())
                                                      .arg(t->getId()));
                } else {
                    /* no AutoTracklet for second */
                    QList<QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> l = t->getObjectsAt(second->getId());
                    for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> p : l)
                        if (p.first->getID() == second->getFrameId())
                            t->removeFromContained(p.first->getID(), p.second->getId());

                    t->addToContained(f,second);
                }
                return true;
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
                    for (auto p: firstTracklet->getContained())
                        secondTracklet->addToContained(p);

                    std::shared_ptr<TrackEvent<Tracklet>> prevEvent = firstTracklet->getPrev();
                    if (prevEvent) {
                        switch (prevEvent->getType()) {
                        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_DEAD:
                        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_LOST:
                        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_ENDOFMOVIE:
                            qDebug() << "previous event was Dead, Lost or EndOfMovie. This should not be possible";
                            return false;
                            break;
                        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_MERGE: {
                            // one next
                            std::shared_ptr<TrackEventMerge<Tracklet>> tem = std::static_pointer_cast<TrackEventMerge<Tracklet>>(prevEvent);
                            tem->setNext(secondTracklet);
                            break; }
                        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_DIVISION: {
                            // multiple next
                            std::shared_ptr<TrackEventDivision<Tracklet>> ted = std::static_pointer_cast<TrackEventDivision<Tracklet>>(prevEvent);
                            QMutableListIterator<std::weak_ptr<Tracklet>> it(*ted->getNext());
                            while (it.hasNext())
                                if (it.next().lock() == firstTracklet)
                                    it.remove();
                            ted->getNext()->push_back(secondTracklet);
                            break; }
                        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_UNMERGE: {
                            // multiple next
                            std::shared_ptr<TrackEventUnmerge<Tracklet>> teu = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(prevEvent);
                            QMutableListIterator<std::weak_ptr<Tracklet>> it(*teu->getNext());
                            while (it.hasNext())
                                if (it.next().lock() == firstTracklet)
                                    it.remove();
                            teu->getNext()->push_back(secondTracklet);
                            break; }
                        }
                    }
                    secondTracklet->setPrev(prevEvent);

                    MessageRelay::emitUpdateStatusBar(QString("Joined tracklets %1 and %2")
                                                      .arg(firstTracklet->getId())
                                                      .arg(secondTracklet->getId()));

                    this->removeTracklet(firstTracklet->getId());
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

/*!
 * \brief adds all Frame/Object-pairs from an AutoTracklet to a Tracklet
 * \param t the Tracklet that the pairs are added to
 * \param at the AutoTracklet that the pairs are from
 */
void Genealogy::allFromAT(std::shared_ptr<Tracklet> t, std::shared_ptr<AutoTracklet> at)
{
    if(!t || !at)
        return;
    for (auto p: at->getComponents().toStdMap()) {
        std::shared_ptr<Frame> f = this->project.lock()->getMovie()->getFrame(p.first);
        QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>> pair(f, p.second);
        t->addToContained(pair);
    }
}

/*!
 * \brief adds all Frame/Object-pairs between two Frames from an AutoTracklet to a Tracklet
 * \param t the Tracklet that the pairs are added to
 * \param at the AutoTracklet that the pairs are from
 * \param from the earliest Frame for adding
 * \param to the latest Frame for adding
 */
void Genealogy::allFromATBetween(std::shared_ptr<Tracklet> t,
                                           std::shared_ptr<AutoTracklet> at,
                                           std::shared_ptr<Frame> from,
                                           std::shared_ptr<Frame> to)
{
    if(!t || !at || !from || !to)
        return;
    for (auto p: at->getComponents().toStdMap()) {
        if (p.first >= 0 && (uint32_t)p.first >= from->getID() && (uint32_t)p.first <= to->getID()) {
            std::shared_ptr<Frame> f = this->project.lock()->getMovie()->getFrame(p.first);
            QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>> pair(f, p.second);
            t->addToContained(pair);
        }
    }
}

/*!
 * \brief adds all Frame/Object-pairs from a given Frame on from an AutoTracklet to a Tracklet
 * \param t the Tracklet that the pairs are added to
 * \param at the AutoTracklet that the pairs are from
 * \param from the earliest Frame for adding
 */
void Genealogy::allFromATFrom(std::shared_ptr<Tracklet> t,
                                        std::shared_ptr<AutoTracklet> at,
                                        std::shared_ptr<Frame> from)
{
    if(!t || !at || !from)
        return;
    for (auto p: at->getComponents().toStdMap()) {
        if (p.first >= 0 && (uint32_t)p.first >= from->getID()) {
            std::shared_ptr<Frame> f = this->project.lock()->getMovie()->getFrame(p.first);
            QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>> pair(f, p.second);
            t->addToContained(pair);
        }
    }
}

/*!
 * \brief adds all Frame/Object-pairs up to a given Frame from an AutoTracklet to a Tracklet
 * \param t the Tracklet that the pairs are added to
 * \param at the AutoTracklet that the pairs are from
 * \param to the latest Frame for adding
 */
void Genealogy::allFromATUntil(std::shared_ptr<Tracklet> t,
                                      std::shared_ptr<AutoTracklet> at,
                                      std::shared_ptr<Frame> to)
{
    if(!t || !at || !to)
        return;
    for (auto p: at->getComponents().toStdMap()) {
        if (p.first >= 0 && (uint32_t)p.first <= to->getID()) {
            std::shared_ptr<Frame> f = this->project.lock()->getMovie()->getFrame(p.first);
            QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>> pair(f, p.second);
            t->addToContained(pair);
        }
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
    for (std::shared_ptr<CellTracker::Tracklet> t: *(g.tracklets))
        strm << "    " << *t;
    return strm;
}
