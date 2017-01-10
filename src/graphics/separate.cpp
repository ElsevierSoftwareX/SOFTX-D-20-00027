#include "separate.h"

#include <QDebug>

namespace CellTracker {

QPair<QPolygonF,QPolygonF> Separate::compute(QPolygonF &poly, QLineF &line) {
    QList<QPointF> l = poly.toList();
    QList<QLineF> lines;

    for (int i = 0; i < l.length(); i++) {
        QPointF p1(l[i]), p2(l[(i+1)%l.length()]);
        if (p1 != p2) { /* remove lines with length 0 */
            QLineF tmpLine(p1,p2);
            lines.push_back(tmpLine);
        }
    }

    QList<QList<QLineF>> polys;
    QList<QLineF> poly1, poly2;
    polys.push_back(poly1);
    polys.push_back(poly2);
    int currPoly = 0;

    for (QLineF polyLine : lines) {
        QLineF lF(polyLine);
        QPointF intersectPoint;
        switch (lF.intersect(line, &intersectPoint)) {
        case QLineF::IntersectType::NoIntersection:        /* lines parallel */
        case QLineF::IntersectType::UnboundedIntersection: /* intersection not in the bounds of the line */
            polys[currPoly] << lF;
            break;
        case QLineF::IntersectType::BoundedIntersection:   /* intersection in the bounds of the line */
            polys[currPoly] << QLineF(lF.p1(),intersectPoint);
            currPoly = (currPoly+1)%polys.length();
            polys[currPoly] << QLineF(intersectPoint,lF.p2());
            break;
        }
    }

    QPolygonF ply1, ply2;
    for (QLineF ln : polys[0])
        ply1 << ln.p1() << ln.p2();
    for (QLineF ln : polys[1])
        ply2 << ln.p1() << ln.p2();

    return QPair<QPolygonF,QPolygonF>(ply1, ply2);
}

}
