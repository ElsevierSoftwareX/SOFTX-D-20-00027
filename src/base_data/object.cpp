#include "object.h"

namespace CellTracker {

Object::Object() : Annotateable(OBJECT_ANNOTATION)
{
}

Object::Object(uint32_t id, uint32_t frameId) : Annotateable(OBJECT_ANNOTATION)
{
    this->id = id;
    this->frameId = frameId;
}

Object::~Object()
{
}

void Object::setId(uint32_t id)
{
    this->id = id;
}

void Object::setCentroid(std::shared_ptr<QPoint> centroid)
{
    this->centroid = centroid;
}

void Object::setBoundingBox(std::shared_ptr<QRect> bbox)
{
    this->boundingBox = bbox;
}

void Object::setOutline(std::shared_ptr<QPolygonF> outline)
{
    this->outline = outline;
}

uint32_t Object::getId() const
{
    return this->id;
}
uint32_t Object::getTrackId() const
{
    return trackId;
}

uint32_t Object::getAutoId() const
{
    return this->autoId;
}

bool Object::isInTracklet() const
{
    return this->trackId != UINT32_MAX;
}

bool Object::isInAutoTracklet() const
{
    return this->autoId != UINT32_MAX;
}

uint32_t Object::getFrameId() const
{
    return this->frameId;
}

void Object::setTrackId(const uint32_t &value)
{
    this->trackId = value;
}

void Object::setAutoId(const uint32_t &autoId)
{
    this->autoId = autoId;
}

std::shared_ptr<QRect> Object::getBoundingBox() const
{
    return this->boundingBox;
}

std::shared_ptr<QPolygonF> Object::getOutline() const
{
    return this->outline;
}

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
    strm << "                  outline: ";
    for (QPointF q: o.outline->toStdVector()){
        strm << "(" << q.x() << "," << q.y() << ")";
    }
    strm << std::endl;
    return strm;

}
