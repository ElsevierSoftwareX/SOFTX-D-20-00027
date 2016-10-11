#include "floodfill.h"

#include <QPolygonF>
#include <QList>
#include <QPoint>
#include <QPainterPath>
#include <QSet>
#include <QImage>

QPolygonF FloodFill::maskToPoly(QList<QPoint> mask)
{
    QPainterPath qpp;
    for (QPoint &p : mask)
        qpp.addRect(p.x(), p.y(), 1, 1);
    return qpp.simplified().toFillPolygon();
}

QPolygonF FloodFill::maskToPoly2(QList<QPoint> mask)
{
    QList<QPoint> points(mask);
    QSet<QPoint> set(points.toSet());
    QList<QPoint> directions = {
        QPoint{-1,0}, QPoint{-1,-1}, QPoint{0,-1}, QPoint{1,-1},
        QPoint{1,0}, QPoint{1,1}, QPoint{0,1}, QPoint{-1,1}
    };
    QPolygon polygon;

    if (mask.size() >= 2) {
        QPoint 	b, c, b0, b1, c1, tmp, currentDir, previousDir;

        int currDir = directions.indexOf(QPoint(-1,0));

        std::sort(points.begin(), points.end(),
                  [](QPoint &a, QPoint &b){ return (a.y() == b.y())?(a.x() > b.x()):(a.y() > b.y()); });

        b0 = points.at(0);

        for (int i=1; i<8; i++) {
            previousDir = directions[currDir];
            currDir = (currDir + 1) % directions.size();
            currentDir = directions[currDir];
            tmp = b0 + currentDir;

            if  (set.contains(tmp)) {
                b1 = tmp;
                c1 = b0 + previousDir;
                polygon.append(b0);
                polygon.append(b1);
                break;
            }
        }

        b = b1;
        c = c1;

        while (true) {
            currDir = directions.indexOf(c-b);

            for (int i=1; i<8; i++) {
                previousDir = directions[currDir];
                currDir = (currDir + 1) % directions.size();
                currentDir = directions[currDir];
                tmp = b + currentDir;

                if (set.contains(tmp)) {
                    c = b + previousDir;
                    b = tmp;
                    polygon.append(b);

                    break;
                }

                if (polygon.size() > 1000)
                    return polygon;
            }

            if (b == b0)
                break;
        }
    }

    return polygon;
}

QList<QPoint> FloodFill::neighbors(QPoint &p)
{
    QList<QPoint> ret;
    int x = p.x();
    int y = p.y();
    int imgHeight = image.height()/scaleFactor;
    int imgWidth = image.width()/scaleFactor;

    switch (connectMode) {
    case C4:
        /* 4 Neighbor */
        if (y - 1 > 0        ) ret.push_back(QPoint(x    , y - 1));
        if (x + 1 < imgWidth ) ret.push_back(QPoint(x + 1, y    ));
        if (y + 1 < imgHeight) ret.push_back(QPoint(x    , y + 1));
        if (x - 1 > 0        ) ret.push_back(QPoint(x - 1, y    ));
        break;
    case C8:
        /* 8 Neighbor */
        if (x - 1 > 0        && y - 1 > 0        ) ret.push_back(QPoint(x - 1, y - 1));
        if (                    y - 1 > 0        ) ret.push_back(QPoint(x    , y - 1));
        if (x + 1 < imgWidth && y - 1 > 0        ) ret.push_back(QPoint(x + 1, y - 1));
        if (x + 1 < imgWidth                     ) ret.push_back(QPoint(x + 1, y    ));
        if (x + 1 < imgWidth && y + 1 < imgHeight) ret.push_back(QPoint(x + 1, y + 1));
        if (                    y + 1 < imgHeight) ret.push_back(QPoint(x    , y + 1));
        if (x - 1 > 0        && y + 1 < imgHeight) ret.push_back(QPoint(x - 1, y + 1));
        if (x - 1 > 0                            ) ret.push_back(QPoint(x - 1, y    ));
        break;
    }

    return ret;
}

uint qHash(const QPointF &val) {
    return qHash(QPair<qreal, qreal>(val.x(), val.y()));
}

QPolygonF FloodFill::compute(QPoint &p, int thresh) {
    QSet<QPoint> stack;
    QSet<QPoint> mask;
    QSet<QPoint> rejected;

    QTransform trans;
    trans = trans.scale(1/scaleFactor, 1/scaleFactor);
    QImage img = image.transformed(trans);

    stack.insert(p);

    QRgb colP = img.pixel(p);
    int grayP = qGray(colP);

    int i = 0;
    while (!stack.empty()) {
        i++;
        QPoint curr = *stack.begin();
        stack.remove(curr);
        mask.insert(curr);

        for (QPoint &n : neighbors(curr)) {
            if (stack.contains(n) || mask.contains(n) || rejected.contains(n))
                continue;

            QRgb colN = img.pixel(n);
            int grayN = qGray(colN);
            int diff = abs(grayP - grayN);
            if (diff < thresh)
                stack.insert(n);
            else
                rejected.insert(n);
        }

        if ((mask.size() + stack.size() + rejected.size()) > (img.height() * img.width() * 0.25))
            break; /* stop this, if the mask gets to big */
    }

    QList<QPoint> l = mask.toList();
    QPolygonF qp = maskToPoly2(mask.toList());

    return qp;
}
