#include "object.h"

namespace CellTracker {

Object::Object()
{
}

Object::Object(uint32_t id)
{
    this->id = id;
}

Object::~Object()
{
}

void Object::setID(uint32_t id)
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

uint32_t Object::getID() const
{
    return this->id;
}
uint32_t Object::getTrackID() const
{
    return trackId;
}

void Object::setTrackID(const uint32_t &value)
{
    trackId = value;
}

}

std::ostream &operator<<(std::ostream &strm, CellTracker::Object &o)
{
    strm << "                Object:" << std::endl;
    strm << "                  boundingBox: ("
         << o.boundingBox->topLeft().x() << ","
         << o.boundingBox->topLeft().x() << ")x("
         << o.boundingBox->bottomRight().x() << ","
         << o.boundingBox->bottomRight().x() << ")" << std::endl;
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
