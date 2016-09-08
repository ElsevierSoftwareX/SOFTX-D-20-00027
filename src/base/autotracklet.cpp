#include "autotracklet.h"
#include "provider/idprovider.h"

#include <QDebug>

namespace CellTracker {

/*!
 * \brief constructor for AutoTracklet::AutoTracklet
 *
 * Creates a new AutoTracklet. Initializes trackId with UINT32_MAX
 */
AutoTracklet::AutoTracklet() :
    trackID(IdProvider::getNewAutoTrackletId()) { }

AutoTracklet::AutoTracklet(int id) :
    trackID(IdProvider::claimAutoTrackletId(id)?id:IdProvider::getNewAutoTrackletId()) { }

AutoTracklet::~AutoTracklet() {
    IdProvider::returnAutoTrackletId(this->trackID);
}

/*!
 * \brief constructs a QPair out of the given std::shared_ptr%s and calls addComponent(QPair) with it
 *
 * \param f The std::shared_ptr<Frame> part of the pair
 * \param o the std::shared_ptr<Object> part of the pair
 */
void AutoTracklet::addComponent(std::shared_ptr<Frame> f, std::shared_ptr<Object> o)
{
    QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>> pair(f, o);
    o->setAutoId(this->getID());
    this->addComponent(pair);
}

/*!
 * \brief inserts the given Object into a QMap, using the ID of the Frame as a key to the QMap
 */
void AutoTracklet::addComponent(QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>> p)
{
    this->components.insert(p.first->getID(), p.second);
}

void AutoTracklet::removeComponent(int key)
{
    this->components.remove(key);
}

void AutoTracklet::removeComponent(std::shared_ptr<Frame> f)
{
    this->components.remove(f->getID());
}

/*!
 * \brief sets the ID of this AutoTracklet
 *
 * \param id the ID to set
 */
void AutoTracklet::setID(int id)
{
    this->trackID = id;
}

/*!
 * \brief returns the ID of this AutoTracklet
 * \return the ID
 */
int AutoTracklet::getID()
{
    return this->trackID;
}

/*!
 * \brief returns the components of this AutoTracklet
 *
 * \return the compo
 */
QMap<int, std::shared_ptr<Object>> AutoTracklet::getComponents() const
{
    return this->components;
}

/*!
 * \brief gives the next TrackEvent for this AutoTracklet
 * \return the next TrackEvent
 */
std::shared_ptr<TrackEvent<AutoTracklet> > AutoTracklet::getNext() const
{
    return next;
}

/*!
 * \brief sets the next TrackEvent for this AutoTracklet
 * \param value the next TrackEvent to set
 */
void AutoTracklet::setNext(const std::shared_ptr<TrackEvent<AutoTracklet> > &value)
{
    next = value;
}

/*!
 * \brief gives the previous TrackEvent for this AutoTracklet
 * \return the previous TrackEvent
 */
std::shared_ptr<TrackEvent<AutoTracklet> > AutoTracklet::getPrev() const
{
    return prev;
}

/*!
 * \brief sets the previous TrackEvent for this AutoTracklet
 * \param value the previous TrackEvent to set
 */
void AutoTracklet::setPrev(const std::shared_ptr<TrackEvent<AutoTracklet> > &value)
{
    prev = value;
}

/*!
 * \brief gives the FrameID for the start of this AutoTracklet
 * \return the FrameID for the start
 */
int AutoTracklet::getStart() {
    return components.firstKey();
}

/*!
 * \brief gives the FrameID for the end of this AutoTracklet
 * \return the FrameID for the end
 */
int AutoTracklet::getEnd() {
    return components.lastKey();
}

/*!
 * \brief gives the length of this AutoTracklet
 * \return the length
 */
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
