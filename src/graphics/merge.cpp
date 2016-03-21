#include "merge.h"

#include <QDebug>
#include <QLine>
#include <QPainterPath>

Merge::Merge() { }

QPolygonF &Merge::compute(QPolygonF &first, QPolygonF &second)
{
    /* find the tangents from the center of the first polygon to the second polygon */
    QPointF c1 = first.boundingRect().center();
    for (QPointF &p : second) {
        QLineF line(c1, p);
        int intersectionCount = 0;
        for (int i = 0; i < second.size(); i++) {
            QLineF segment(second[i], second[(i+1)%second.size()]);
            if (line.intersect(segment, nullptr) == QLineF::IntersectType::BoundedIntersection)
                intersectionCount++;
        }
        qDebug() << line << "has" << intersectionCount << "intersections with second";
    }
}
