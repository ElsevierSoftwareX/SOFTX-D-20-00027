#ifndef OBJECT_H
#define OBJECT_H

#include <cstdint>
#include <QPoint>
#include <QRect>
#include <QPolygon>

class Object
{
public:
    Object();

private:
    uint32_t id;
    QPoint centroid;
    QRect boundingBox;
    QPolygon outline;
    uint32_t trackId;
};

#endif // OBJECT_H
