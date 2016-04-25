#include "provider/idprovider.h"
#include "tracklet.h"
#include "tracked/trackeventdead.h"
#include "tracked/trackeventdivision.h"
#include "tracked/trackeventendofmovie.h"
#include "tracked/trackeventlost.h"
#include "tracked/trackeventmerge.h"
#include "tracked/trackeventunmerge.h"

#include <functional>
#include <QList>
#include <QStringList>
#include <QSet>

namespace CellTracker {

/*!
 * \brief constructs a new Tracklet
 */
Tracklet::Tracklet() :
    QObject(0),
    Annotateable(),
    id(IdProvider::getNewTrackletId()) {}

/*!
 * \brief destructs a Tracklet
 */
Tracklet::~Tracklet() {
    IdProvider::returnTrackletId(this->id);
}

/*!
 * \brief returns a QList of QPair%s of Frames and Objects at a certain Frame (this should only be one)
 * \param frameId the Frame
 * \return a QList of QPair%s of Frames and Objects at that Frame
 */
QList<QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> Tracklet::getObjectsAt(int frameId) const
{
    return this->contained.values(frameId);
}

/*!
 * \brief returns a QHash containing QPairs of Frame%s and Object%s as QHash.second
 * \return the QHash containing all Frame-Object relations in this Tracklet
 */
QHash<int,QPair<std::shared_ptr<Frame>, std::shared_ptr<Object> > > Tracklet::getContained() const
{
    return contained;
}

/*!
 * \brief checks whether this Tracklet has an Object at a certain Frame
 * \param objId the Object to check for
 * \param frameId the Frame at which to check
 * \return true if there is such an Object at that Frame, false otherwise
 */
bool Tracklet::hasObjectAt(int objId, int frameId) {
    QPair<int,int> p(frameId, objId);
    return contained.contains(qHash<int,int>(p));
}

/*!
 * \brief sets the QHash of contained QPairs
 * \param value the new QHash for contained QPairs
 */
void Tracklet::setContained(const QHash<int,QPair<std::shared_ptr<Frame>, std::shared_ptr<Object> > > &value)
{
    contained = value;
}

/*!
 * \brief Adds a QPair of a Frame and an Object to this Tracklet
 * \param f the Frame
 * \param o the Object
 */
void Tracklet::addToContained(const std::shared_ptr<Frame> &f, const std::shared_ptr<Object> &o)
{
    QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> pair(f,o);
    this->addToContained(pair);
}

/*!
 * \brief Adds a QPair of a Frame and an Object to this Tracklet
 * \param p the Pair of a Frame and an Object
 */
void Tracklet::addToContained(const QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>> p)
{
    QPair<int,int> idPair (p.first->getID(), p.second->getId());
    p.second->setTrackId(this->id);
    this->contained.insert(qHash<int,int>(idPair), p);
}

/*!
 * \brief removes an Object at a certain Frame from this Tracklet
 * \param frameId the Frame
 * \param objId the ObjectID of the Obejct to remove at the Frame
 */
void Tracklet::removeFromContained(int frameId, uint32_t objId)
{
    QPair<int,int> idPair (frameId, objId);
    this->contained.value(qHash<int,int>(idPair)).second->setTrackId(UINT32_MAX);
    this->contained.remove(qHash<int,int>(idPair));
}

/*!
 * \brief returns the end of this Tracklet (i.e. the QPair, whose Frame has the highest FrameID)
 * \return the QPair representing the last Frame/Object-relation in this Tracklet
 */
QPair<std::shared_ptr<Frame>, std::shared_ptr<Object> > Tracklet::getEnd() const
{
    QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> ret(nullptr,nullptr);
    for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> p: this->contained) {
        if (ret.first == nullptr || p.first->getID() > ret.first->getID())
            ret = p;
    }
    return ret;
}

/*!
 * \brief returns the start of this Tracklet (i.e. the QPair, whose Frame has the lowest FrameID)
 * \return the QPair representing the first Frame/Object-relation in this Tracklet
 */
QPair<std::shared_ptr<Frame>, std::shared_ptr<Object> > Tracklet::getStart() const
{
    QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> ret(nullptr,nullptr);
    for (QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>> p: this->contained) {
        if (ret.first == nullptr || p.first->getID() < ret.first->getID())
            ret = p;
    }
    return ret;
}

/*!
 * \brief returns the next TrackEvent
 * \return the next TrackEvent
 */
std::shared_ptr<TrackEvent<Tracklet> > Tracklet::getNext() const
{
    return next;
}

/*!
 * \brief returns the previous TrackEvent
 * \return the previous TrackEvent
 */
std::shared_ptr<TrackEvent<Tracklet> > Tracklet::getPrev() const
{
    return prev;
}

/*!
 * \brief sets the next TrackEvent
 * \param value the next TrackEvent to set
 */
void Tracklet::setNext(const std::shared_ptr<TrackEvent<Tracklet> > &value)
{
    next = value;
}

/*!
 * \brief sets the previous TrackEvent
 * \param value the previous TrackEvent to set
 */
void Tracklet::setPrev(const std::shared_ptr<TrackEvent<Tracklet> > &value)
{
    prev = value;
}

/*!
 * \brief returns the TrackID of this Tracklet
 * \return the TrackID of this Tracklet
 */
int Tracklet::getId() const
{
    return id;
}

/*!
 * \brief sets the TrackID of this Tracklet
 * \param value the TrackID to set
 * \warning Do not use this, as it circumvents the IdProvider, which may lead to serious breakage
 */
void Tracklet::setId(int value)
{
    id = value;
}

QString Tracklet::qmlId() {
    return QString::fromStdString(std::to_string(id));
}

QString Tracklet::qmlStart() {
    return QString::fromStdString(std::to_string(getStart().first->getID()));
}

QString Tracklet::qmlEnd() {
    return QString::fromStdString(std::to_string(getEnd().first->getID()));
}

QString Tracklet::qmlPrevious() {
    if (prev) {
        QStringList p;
        switch (prev->getType()) {
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_DEAD: {
            std::shared_ptr<TrackEventDead<Tracklet>> ted = std::static_pointer_cast<TrackEventDead<Tracklet>>(prev);
            std::shared_ptr<Tracklet> pT = ted->getPrev().lock();
            if (pT)
                p.push_back(QString::fromStdString(std::to_string(pT->getId())));
            break; }
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_ENDOFMOVIE: {
            std::shared_ptr<TrackEventEndOfMovie<Tracklet>> teeom = std::static_pointer_cast<TrackEventEndOfMovie<Tracklet>>(prev);
            std::shared_ptr<Tracklet> pT = teeom->getPrev().lock();
            if (pT)
                p.push_back(QString::fromStdString(std::to_string(pT->getId())));
            break; }
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_LOST: {
            std::shared_ptr<TrackEventLost<Tracklet>> tel = std::static_pointer_cast<TrackEventLost<Tracklet>>(prev);
            std::shared_ptr<Tracklet> pT = tel->getPrev().lock();
            if (pT)
                p.push_back(QString::fromStdString(std::to_string(pT->getId())));
            break; }
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_DIVISION: {
            std::shared_ptr<TrackEventDivision<Tracklet>> ted = std::static_pointer_cast<TrackEventDivision<Tracklet>>(prev);
            std::shared_ptr<Tracklet> pT = ted->getPrev();
            if (pT)
                p.push_back(QString::fromStdString(std::to_string(pT->getId())));
            break; }
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_UNMERGE: {
            std::shared_ptr<TrackEventUnmerge<Tracklet>> teu = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(prev);
            std::shared_ptr<Tracklet> pT = teu->getPrev();
            if (pT)
                p.push_back(QString::fromStdString(std::to_string(pT->getId())));
            break; }
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_MERGE: {
            std::shared_ptr<TrackEventMerge<Tracklet>> tem = std::static_pointer_cast<TrackEventMerge<Tracklet>>(prev);
            std::shared_ptr<QList<std::shared_ptr<Tracklet>>> pTs = tem->getPrev();
            for (std::shared_ptr<Tracklet> pT : *pTs)
                p.push_back(QString::fromStdString(std::to_string(pT->getId())));
            break; }
        }
        return p.join(", ");
    } else {
        return QString("-");
    }
}

QString Tracklet::qmlNext() {
    if (next) {
        QStringList n;
        switch (next->getType()) {
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_DEAD:
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_ENDOFMOVIE:
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_LOST:
            n.push_back(QString("-"));
            break;
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_MERGE: {
            std::shared_ptr<TrackEventMerge<Tracklet>> tem = std::static_pointer_cast<TrackEventMerge<Tracklet>>(next);
            std::shared_ptr<Tracklet> nT = tem->getNext();
            if (nT)
                n.push_back(QString::fromStdString(std::to_string(nT->getId())));
            break; }
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_DIVISION: {
            std::shared_ptr<TrackEventDivision<Tracklet>> ted = std::static_pointer_cast<TrackEventDivision<Tracklet>>(next);
            std::shared_ptr<QList<std::shared_ptr<Tracklet>>> nTs = ted->getNext();
            for (std::shared_ptr<Tracklet> nT : *nTs)
                n.push_back(QString::fromStdString(std::to_string(nT->getId())));
            break; }
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_UNMERGE: {
            std::shared_ptr<TrackEventUnmerge<Tracklet>> teu = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(next);
            std::shared_ptr<QList<std::shared_ptr<Tracklet>>> nTs = teu->getNext();
            for (std::shared_ptr<Tracklet> nT : *nTs)
                n.push_back(QString::fromStdString(std::to_string(nT->getId())));
            break; }
        }
        return n.join(", ");
    } else {
        return QString("-");
    }
}

QString Tracklet::qmlStatus() {
    if (next == nullptr)
        return QString("open");
    switch (next->getType()) {
    case TrackEvent<Tracklet>::EVENT_TYPE_DIVISION:   return QString("division");
    case TrackEvent<Tracklet>::EVENT_TYPE_MERGE:      return QString("merge");
    case TrackEvent<Tracklet>::EVENT_TYPE_UNMERGE:    return QString("unmerge");
    case TrackEvent<Tracklet>::EVENT_TYPE_LOST:       return QString("lost");
    case TrackEvent<Tracklet>::EVENT_TYPE_DEAD:       return QString("dead");
    case TrackEvent<Tracklet>::EVENT_TYPE_ENDOFMOVIE: return QString("end of movie");
    }
    return QString("unhandled case in %1:%2").arg(__FILE__).arg(__LINE__);
}

QString Tracklet::qmlTAnno() {
    QSet<QString> qs;
    for (std::shared_ptr<Annotation> a : *getAnnotations())
        qs.insert(QString::fromStdString(std::to_string(a->getId())));

    if (qs.size() == 0)
        return QString("-");
    return QStringList(QStringList::fromSet(qs)).join(", ");
}

QString Tracklet::qmlOAnno() {
    QSet<QString> qs;
    for (QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>> o : contained.values()) {
        std::shared_ptr<Object> obj = o.second;
        if (obj->isAnnotated())
            for (std::shared_ptr<Annotation> a : *obj->getAnnotations())
                qs.insert(QString::fromStdString(std::to_string(a->getId())));
    }
    if (qs.size() == 0)
        return QString("-");
    return QStringList(QStringList::fromSet(qs)).join(", ");
}

}

std::ostream &operator<<(std::ostream &strm, CellTracker::Tracklet &t)
{
    strm << "Tracklet: ";
    strm << std::endl;
    strm << "  id: " << t.id << std::endl;
    strm << "  next: " << t.next << std::endl;
    strm << "  contained: ";
    for (QPair<std::shared_ptr<CellTracker::Frame>,std::shared_ptr<CellTracker::Object>> p: t.contained) {
        strm << "(" << p.first->getID() << "," << p.second->getId() << ") ";
    }
    strm << std::endl;
    return strm;
}
