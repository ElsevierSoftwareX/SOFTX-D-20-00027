#ifndef SEPARATE_H
#define SEPARATE_H

#include <QLine>
#include <QPair>
#include <QPolygon>

namespace CellTracker {
class Separate
{
public:
    Separate();

    static QPair<QPolygonF,QPolygonF> compute(QPolygon &, QLine &);
};

}
#endif // SEPARATE_H
