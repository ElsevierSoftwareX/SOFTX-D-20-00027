#include "merge.h"

#include <QDebug>
#include <QLine>
#include <QPainterPath>

Merge::Merge() { }



QPolygonF Merge::compute(QPolygonF &first, QPolygonF &second) {
    /* find the tangents from the center of the first polygon to the second polygon */
    QPointF c1 = first.boundingRect().center();
    QPointF c2 = second.boundingRect().center();
    QPair<QPointF, QPointF> tps1 = tangentPoints(c1, second);
    QPair<QPointF, QPointF> tps2 = tangentPoints(c2, first);

    QLineF t11(c1, tps1.first);
    QLineF t12(c1, tps1.second);
    QLineF t21(c2, tps2.first);
    QLineF t22(c2, tps2.second);

    QPointF intersection1, intersection2;
    t11.intersect(t21, &intersection1);
    t11.intersect(t22, &intersection2);
    qDebug() << intersection1 << isBetween(c1, c2, intersection1);
    qDebug() << intersection1 << isBetween(c1, c2, intersection2);

    /*! \todo find intersection of the tangents */
    /*! \todo determine, which points lie on the same side */
    /*! \todo build curve between those points */
    /*! \todo segment curve */
    /*! \todo add curves to polygon and remove points lying inside */
}

QPair<QPointF, QPointF> Merge::tangentPoints(QPointF &point, QPolygonF &poly) {
    qDebug() << "tangentPoints from" << point << "to poly of size" << poly.size();
    QPair<QPointF, QPointF> tp;
    QList<QPointF> tpList;
    for (QPointF &p : poly) {
        QLineF line(point, p);
        line.setLength(5000); /* well… infinity seems to be 5k today */
        int intersectionCount = 0;
        for (int i = 0; i < poly.size(); i++) {
            if (poly[i] == poly[(i+1)%poly.size()]) /* ignore for closed polygon */
                continue;
            QLineF segment(poly[i], poly[(i+1)%poly.size()]);
            if (line.intersect(segment, nullptr) == QLineF::IntersectType::BoundedIntersection)
                intersectionCount++;
        }
        if (intersectionCount == 0 && !tpList.contains(p))
            tpList.push_back(p);
    }
    if (tpList.size() == 2) {
        tp.first = tpList[0];
        tp.second = tpList[1];
    } else {
        qDebug() << "determining the tangent points failed:" << tpList;
    }

    return tp;
}

/*!
 * \brief Merge::isBetween checks, whether point c lies between point a and b
 */
bool Merge::isBetween(QPointF &a, QPointF &b, QPointF &c)
{
    /*! \todo */
    return false;
}
