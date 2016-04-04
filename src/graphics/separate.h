#ifndef SEPARATE_H
#define SEPARATE_H

#include <QLineF>
#include <QPair>
#include <QPolygonF>

namespace CellTracker {
class Separate
{
public:
    Separate() = default;

    static QPair<QPolygonF,QPolygonF> compute(QPolygonF&, QLineF&);
};

}
#endif // SEPARATE_H
