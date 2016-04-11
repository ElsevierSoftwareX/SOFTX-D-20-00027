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
    /* find the tangents from the center of the first polygon to the second polygon */

    /* first, if the polygon is build counter-clock-wise, invert it (needed for merging the segments later on) */
    QPolygonF p1 = first;
    if (polygonRotation(p1) < 0)
        std::reverse(p1.begin(), p1.end());
    QPolygonF p2 = second;
    if (polygonRotation(p2) < 0)
        std::reverse(p2.begin(), p2.end());

    /* use the centers of the bounding boxes as center and calculate tangents from those to the other polygon */
    QPointF c1 = p1.boundingRect().center();
    QPointF c2 = p2.boundingRect().center();
    QPair<QPointF, QPointF> tps1 = tangentPoints(c1, p2);
    QPair<QPointF, QPointF> tps2 = tangentPoints(c2, p1);

    /* tangents from left to right */
    QLineF t1L(c1, tps1.first);
    QLineF t1R(c1, tps1.second);

    /* tangents from right to left */
    QLineF t2L(c2, tps2.first);
    QLineF t2R(c2, tps2.second);

    /* left intersection */
    QPointF iL;
    if (t1L.intersect(t2R, &iL) != QLineF::IntersectType::BoundedIntersection)
        qDebug() << "couldn't determine intersection on the left side";

    /* right intersection */
    QPointF iR;
    if (t1R.intersect(t2L, &iR) != QLineF::IntersectType::BoundedIntersection)
        qDebug() << "couldn't determine intersection on the right side";

    /* construct the new segments, that are about to be used */
    QPolygonF newSegmentL = QPolygonF::fromStdVector({tps1.first, iL, tps2.second});
    QPolygonF newSegmentR = QPolygonF::fromStdVector({tps2.first, iR, tps1.second});

    /* check that the intersections aren't the same */
    /*! \todo maybe drop this check, as there may be cornercases where this is necessary */
    if (iL == iR) {
        qDebug() << "intersection point are the same (this shouldn't be possible";
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
    auto last = std::unique(n.begin(), n.end());
    n.erase(last, n.end());

    return n;
}

QPair<QPointF, QPointF> Merge::tangentPoints(QPointF &point, QPolygonF &poly) {
    QPair<QPointF, QPointF> tp;
    QList<QPointF> frontList;
    /* first extract the points that are directly facing the center point */
    for (QPointF &p : poly) {
        QLineF line(point, p);
        int intersectionCount = 0;
        for (int i = 0; i < poly.size(); i++) {
            int j = (i+1)%poly.size();
            if (poly[i] == poly[j]) /* ignore for closed polygon */
                continue;
            QLineF segment(poly[i], poly[j]);
            if (line.intersect(segment, nullptr) == QLineF::IntersectType::BoundedIntersection)
                intersectionCount++;
        }
        if (intersectionCount == 2 && !frontList.contains(p)) /* one intersection with each of the neighboring lines */
            frontList.push_back(p);
    }
    /* now we've got a list of points facing the center in the polygon, prune the ones that aren't tangents */
    /*! \todo only first and last? */
    QList<QPointF> tpList;
    for (QPointF &p : frontList) {
        QLineF line(point, p);
        /* we have to extend the line here as we want to know, if it hits a segment of the polygon that lies behind it */
        /* beware that this introduces inacurracy */
        line.setLength(500000); /* maybe use a constant here, but could overflow in some later calculations */
        int intersectionCount = 0;
        for (int i = 0; i < poly.size(); i++) {
            int j = (i+1)%poly.size();
            if (poly[i] == poly[j])
                continue;
            if (poly[i] == p || poly[j] == p)
                continue;
            QLineF segment(poly[i], poly[j]);
            if (line.intersect(segment, nullptr) == QLineF::IntersectType::BoundedIntersection)
                intersectionCount++;
        }
        if (intersectionCount == 0)
            tpList.append(p);
    }
    if (tpList.size() == 2) {
        QLineF base(point, poly.boundingRect().center());
        QLineF l1(point, tpList[0]), l2(point, tpList[1]);
        qreal al1 = base.angleTo(l1),
              al2 = base.angleTo(l2);
        if (0 < al1 && al1 < 180 && 180 < al2 && al2 < 360) {
            tp = { tpList[0], tpList[1] };
        } else if (0 < al2 && al2 < 180 && 180 < al1 && al1 < 360){
            tp = { tpList[1], tpList[0] };
        } else {
            qDebug() << "one of the tangents equals the line between the two centers";
        }
    } else {
        qDebug() << "found" << tpList.size() << "tangent points:" << tpList;
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
