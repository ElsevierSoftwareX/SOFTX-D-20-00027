#include "object.h"

Object::Object()
{
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

uint32_t Object::getID()
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



std::ostream &operator<<(std::ostream &strm, Object &o)
{
    strm << "                Object:\n";
    strm << "                  boundingBox: ("
         << o.boundingBox->topLeft().x() << ","
         << o.boundingBox->topLeft().x() << ")x("
         << o.boundingBox->bottomRight().x() << ","
         << o.boundingBox->bottomRight().x() << ")\n";
    strm << "                  centroid: (" << o.centroid->x() << "," << o.centroid->y() << ")\n";
    strm << "                  id: " << o.id << "\n";
    strm << "                  trackId: " << o.trackId << "\n";
    strm << "                  outline: ";
    for (QPointF q: o.outline->toStdVector()){
        strm << "(" << q.x() << "," << q.y() << ")";
    }
    strm << "\n";
    return strm;

}
