#ifndef MERGE_H
#define MERGE_H

#include <QLineF>
#include <QPair>
#include <QPolygonF>

class Merge
{
public:
    Merge() = default;
    static QPolygonF compute(QPolygonF&, QPolygonF&);
    static QPolygonF segment(const QPolygonF &poly, const QPointF &start, const QPointF &end);

private:
    static QPair<QPointF,QPointF> tangentPoints(QPointF&, QPolygonF&);
    static int polygonRotation(QPolygonF& poly);
    static QList<QPair<QPointF, qreal>> angleToAll(QPointF &c, QPolygonF &poly);
    static QPair<QPointF, QPointF> extremaToBase(QList<QPair<QPointF, qreal>> points);
    static QList<QLineF> segmentsOf(QPolygonF &p);
};

#endif // MERGE_H
