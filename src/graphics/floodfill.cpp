#include "floodfill.h"

#include <QDebug>
#include <QStack>

#include <mutex>

QPolygonF FloodFill::maskToPoly(QList<QPoint> mask)
{
    QPainterPath qpp;
    for (QPoint &p : mask)
        qpp.addRect(p.x(), p.y(), 1, 1);
    return qpp.simplified().toFillPolygon();
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
    }

    QList<QPoint> l = mask.toList();
    std::sort(l.begin(), l.end(), [](const QPointF &a, const QPointF &b){ return (a.x() == b.x())?a.x() > b.x():a.y() > b.y(); });
    QPolygonF qp = maskToPoly(mask.toList());

    return qp;
}
