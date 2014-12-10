#include "object.h"

Object::Object()
{
}

Object::~Object()
{
    delete centroid;
    delete boundingBox;
    delete outline;
}

void Object::setID(uint32_t id)
{
    this->id = id;
}

void Object::setTrackID(uint32_t trackID)
{
    this->trackId = trackID;
}

void Object::setCentroid(QPoint *centroid)
{
    this->centroid = centroid;
}

void Object::setBoundingBox(QRect *bbox)
{
    this->boundingBox = bbox;
}

void Object::setOutline(QPolygonF *outline)
{
    this->outline = outline;
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
