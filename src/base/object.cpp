#include "object.h"

namespace CellTracker {

/*!
 * \brief creates a new Object
 *
 * objectID, channelID, sliceID, frameID, trackID and autoTrackletID are
 * initialized with UINT32_MAX
 */
Object::Object() :
    Annotateable(),
    id(UINT32_MAX),
    chanId(UINT32_MAX),
    sliceId(UINT32_MAX),
    frameId(UINT32_MAX),
    trackId(UINT32_MAX),
    autoId(UINT32_MAX) {}

/*!
 * \brief creates a new Object
 *
 * \param id the objectID for this Object
 * \param chanId the channelID for this Object
 * \param sliceId the sliceID for this Object
 * \param frameId the frameID for this Object
 *
 * trackID and autoTrackletID are initialized with UINT32_MAX to initcate, that
 * they are uninitialized
 */
Object::Object(uint32_t id, uint32_t chanId, uint32_t sliceId, uint32_t frameId) :
    Annotateable(),
    id(id),
    chanId(chanId),
    sliceId(sliceId),
    frameId(frameId),
    trackId(UINT32_MAX),
    autoId(UINT32_MAX) {}

/*!
 * \brief sets the ID of this Object
 * \param id the ID to set
 */
void Object::setId(uint32_t id)
{
    this->id = id;
}

/*!
 * \brief sets the Centroid (i.e. central point) of this Object
 * \param centroid the Centroid to set
 */
void Object::setCentroid(std::shared_ptr<QPoint> centroid)
{
    this->centroid = centroid;
}

/*!
 * \brief sets the BoundingBox (i.e. surrounding box) of the Object
 * \param bbox the BoundingBox to set
 */
void Object::setBoundingBox(std::shared_ptr<QRect> bbox)
{
    this->boundingBox = bbox;
}

/*!
 * \brief sets the Outline of the Object
 * \param outline the Outline of the Object
 */
void Object::setOutline(std::shared_ptr<QPolygonF> outline)
{
    this->outline = outline;
}

/*!
 * \brief returns the ID of the Object
 * \return the objectID of the Object
 */
uint32_t Object::getId() const
{
    return this->id;
}

/*!
 * \brief returns the TrackID of the Object
 * \return the objectID of the Object
 */
uint32_t Object::getTrackId() const
{
    return trackId;
}

/*!
 * \brief returns the SliceID of the Object
 * \return the sliceID of the Object
 */
uint32_t Object::getSliceId() const
{
    return sliceId;
}

/*!
 * \brief returns the ChannelID of the Object
 * \return the channelID of the Object
 */
uint32_t Object::getChannelId() const
{
    return chanId;
}

/*!
 * \brief returns the AutoTrackletID of the Object
 * \return the autoTrackletID of the Object
 */
uint32_t Object::getAutoId() const
{
    return this->autoId;
}

/*!
 * \brief returns, whether the Object is contained in a Tracklet
 * \return true if it is, false if it isn't
 */
bool Object::isInTracklet() const
{
    return this->trackId != UINT32_MAX;
}

/*!
 * \brief returns, whether the Object is contained in a AutoTracklet
 * \return true if it is, false if it isn't
 */
bool Object::isInAutoTracklet() const
{
    return this->autoId != UINT32_MAX;
}

/*!
 * \brief returns the FrameID of the Object
 * \return the frameID of the Object
 */
uint32_t Object::getFrameId() const
{
    return this->frameId;
}

/*!
 * \brief sets the TrackID of the Object
 * \param value the trackID to set
 */
void Object::setTrackId(const uint32_t &value)
{
    this->trackId = value;
}

/*!
 * \brief sets the AutoTrackletID of the Object
 * \param autoId the autoTrackletID to set
 */
void Object::setAutoId(const uint32_t &autoId)
{
    this->autoId = autoId;
}

/*!
 * \brief returns the BoundingBox of the Object
 * \return the BoundingBox of the Object
 */
std::shared_ptr<QRect> Object::getBoundingBox() const
{
    return this->boundingBox;
}

/*!
 * \brief returns the Outline of the Object
 * \return the Outline of the Object
 */
std::shared_ptr<QPolygonF> Object::getOutline() const
{
    return this->outline;
}

/*!
 * \brief returns the Centroid of the Object
 * \return the Centroid of the Object
 */
std::shared_ptr<QPoint> Object::getCentroid() const
{
    return this->centroid;
}

}

std::ostream &operator<<(std::ostream &strm, CellTracker::Object &o)
{
    strm << "                Object:" << std::endl;
    strm << "                  boundingBox: ("
         << o.boundingBox->topLeft().x() << ","
         << o.boundingBox->topLeft().y() << ")x("
         << o.boundingBox->bottomRight().x() << ","
         << o.boundingBox->bottomRight().y() << ")" << std::endl;
    strm << "                  centroid: (" << o.centroid->x() << "," << o.centroid->y() << ")" << std::endl;
    strm << "                  id: " << o.id << std::endl;
    strm << "                  trackId: " << o.trackId << std::endl;
    strm << "                  frameId: " << o.frameId << std::endl;
    strm << "                  outline: ";
    for (QPointF q: o.outline->toStdVector()){
        strm << "(" << q.x() << "," << q.y() << ")";
    }
    strm << std::endl;
    return strm;

}
