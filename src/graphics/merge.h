#ifndef MERGE_H
#define MERGE_H

#include <QPair>
#include <QPolygonF>

class Merge
{
public:
    Merge();
    static QPolygonF compute(QPolygonF&, QPolygonF&);
private:
    static QPair<QPointF,QPointF> tangentPoints(QPointF&, QPolygonF&);
    static bool isBetween(QPointF&, QPointF&, QPointF&);
};

#endif // MERGE_H
