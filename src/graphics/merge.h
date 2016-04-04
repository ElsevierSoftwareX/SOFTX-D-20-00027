#ifndef MERGE_H
#define MERGE_H

#include <QPair>
#include <QPolygonF>

class Merge
{
public:
    Merge() = default;
    static QPolygonF compute(QPolygonF&, QPolygonF&);
private:
    static QPair<QPointF,QPointF> tangentPoints(QPointF&, QPolygonF&);
    static int polygonRotation(QPolygonF& poly);
public:
    static QPolygonF segment(const QPolygonF &poly, const QPointF &start, const QPointF &end);

};

#endif // MERGE_H
