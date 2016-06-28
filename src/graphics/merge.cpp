#include "merge.h"

#include <QDebug>
#include <QLine>
#include <QPainterPath>

QPolygonF Merge::segment(const QPolygonF &poly, const QPointF &begin, const QPointF &end) {
    QPolygonF ret;
    bool a = false;
    for (int i = 0; i < 2*poly.size(); i++) { /* worst case: begin is last point, end is second to last */
        const QPointF p = poly.at(i%poly.size());
        if (p == begin) a = true;
        if (a) ret.push_back(p);
        if (a && p == end) break;
    }
    return ret;
}

QPolygonF Merge::compute(QPolygonF &first, QPolygonF &second) {
    if (first == second || first.isEmpty() || second.isEmpty())
        return QPolygon();

    /* first, if the polygon is build counter-clock-wise, invert it (needed for merging the segments later on) */
    QPolygonF p1 = first;
    if (polygonRotation(p1) < 0)
        std::reverse(p1.begin(), p1.end());
    QPolygonF p2 = second;
    if (polygonRotation(p2) < 0)
        std::reverse(p2.begin(), p2.end());

    /* sanitize input polygons */
    auto last = std::unique(p1.begin(), p1.end());
    p1.erase(last, p1.end());
    last = std::unique(p2.begin(), p2.end());
    p2.erase(last, p2.end());

    /*! \todo see, if there are any points that these polygons have in common
     *  and use those to join them, if more than two */
    /* find the tangents from the center of the first polygon to the second polygon */

    /* use the centers of the bounding boxes as center and calculate tangents from those to the other polygon */
    QPointF c1 = p1.boundingRect().center();
    QPointF c2 = p2.boundingRect().center();
    QPair<QPointF, QPointF> tps1 = tangentPoints(c1, p2);
    QPair<QPointF, QPointF> tps2 = tangentPoints(c2, p1);

    if (tps1.first.isNull() || tps1.second.isNull() || tps2.first.isNull() || tps2.second.isNull())
        return QPolygon();

    /* tangents from left to right */
    QLineF t1L(c1, tps1.first);
    QLineF t1R(c1, tps1.second);

    /* tangents from right to left */
    QLineF t2L(c2, tps2.first);
    QLineF t2R(c2, tps2.second);

    /* left intersection */
    QPointF iL;
    /* both BoundedIntersection and UnboundedIntersection are good */
    if (t1L.intersect(t2R, &iL) == QLineF::IntersectType::NoIntersection) {
        qDebug() << "couldn't determine intersection on the left side";
        return QPolygon();
    }

    /* right intersection */
    QPointF iR;
    if (t1R.intersect(t2L, &iR) == QLineF::IntersectType::NoIntersection) {
        qDebug() << "couldn't determine intersection on the right side";
        return QPolygon();
    }

    /* construct the new segments, that are about to be used */
    QPolygonF newSegmentL = QPolygonF::fromStdVector({tps1.first, iL, tps2.second});
    QPolygonF newSegmentR = QPolygonF::fromStdVector({tps2.first, iR, tps1.second});

    /* check that the intersections aren't the same */
    /*! \todo maybe drop this check, as there may be cornercases where this is necessary */
    if (iL == iR) {
        qDebug() << "intersection points are the same (this shouldn't be possible)";
        return QPolygonF();
    }

    /*! \todo build curve between those points */
    /*! \todo segment curve */

    /* merge everthing */
    QPolygonF n;
    n.append(newSegmentL);
    n.append(segment(p1, newSegmentL.last(), newSegmentR.first()));
    n.append(newSegmentR);
    n.append(segment(p2, newSegmentR.last(), newSegmentL.first()));

    /* remove duplicate points */
    last = std::unique(n.begin(), n.end());
    n.erase(last, n.end());

    return n;
}

QList<QLineF> segmentsOf(QPolygonF &p) {
    int i, j;
    int size = p.size();
    QList<QLineF> segments;

    for (i = 0; i < size; i++) {
        j = (i+1)%size;

        if (p[i] == p[j]) /* closed polygon */
            continue;

        QLineF l(p[i], p[j]);
        segments.push_back(l);
    }
    return segments;
}

QList<QPair<QPointF, qreal>> Merge::angleToAll(QPointF &c, QPolygonF &poly) {
    QPointF center = poly.boundingRect().center();
    QLineF base(c, center);

    QList<QPair<QPointF, qreal>> angles;
    for (QPointF &p : poly) {
        QLineF curr(c, p);
        qreal angle = base.angleTo(curr);

        angles.push_back(QPair<QPointF, qreal>(p, angle));
    }
    return angles;
}

QPair<QPointF, QPointF> Merge::extremaToBase(QList<QPair<QPointF, qreal>> points) {
    using namespace std;
    QPair<QPointF, qreal> left, right;

    for (QPair<QPointF, qreal> pair : points) {
        qreal curr_angle = pair.second;
        /* find the points nearest to 90 and 270 */
        qreal diff90 = fabs(90-curr_angle);
        qreal diff270 = fabs(270-curr_angle);
        if (diff90 < fabs(90 - left.second))
            left = pair;
        if (diff270 < fabs(270 - fabs(right.second)))
            right = pair;
    }

    return QPair<QPointF, QPointF>(left.first, right.first);
}

QPair<QPointF, QPointF> Merge::tangentPoints(QPointF &point, QPolygonF &poly) {
    QPair<QPointF, QPointF> tp;

    QList<QPair<QPointF, qreal>> angles = angleToAll(point, poly);
    tp = extremaToBase(angles);

    /* sort the tangent points such that the first one is left of the base line between the centers */
    QLineF base(point, poly.boundingRect().center());
    QLineF l1(point, tp.first), l2(point, tp.second);
    qreal al1 = base.angleTo(l1),
            al2 = base.angleTo(l2);
    if (0 < al1 && al1 < 180 && 180 < al2 && al2 < 360) {
        tp = { tp.first, tp.second };
    } else if (0 < al2 && al2 < 180 && 180 < al1 && al1 < 360){
        tp = { tp.second, tp.first };
    } else {
        qDebug() << "one of the tangents equals the line between the two centers";
    }

    return tp;
}

/*!
 * \brief Merge::polygonRotation calculates, wheter a given polygon is constructed clock-wise or counter-clock-wise
 * \param poly the QPolygonF to check
 * \return a positive integer, if this polygon is build clock-wise, a negative integer if it is build counter-clock-wise
 */
int Merge::polygonRotation(QPolygonF &poly)
{
    int sum = 0;
    for (int i = 0; i < poly.size(); i++) {
        int j = (i+1)%poly.size();
        QPointF p1 = poly[i],
                p2 = poly[j];
        sum += (p2.x()-p1.x())*(p2.y()+p1.y());
    }
    return sum;
}
