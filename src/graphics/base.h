#ifndef BASE_H
#define BASE_H

#include <memory>

#include "base/object.h"

namespace CellTracker {
class Base
{
public:
    Base() = delete;

    static bool cut(QPolygonF& objectPoly, QPolygonF &linePoly);
    static bool cut(std::shared_ptr<Object> object, QPolygonF &linePoly);

    static bool pointInObject(qreal x, qreal y);
    static bool pointInObject(QPointF &&p);

    static std::shared_ptr<Object> objectCutByLine(QLineF &line);
};
}

#endif // BASE_H
