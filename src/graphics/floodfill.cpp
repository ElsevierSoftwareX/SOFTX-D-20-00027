#include "floodfill.h"

#include <QDebug>
#include <QStack>

#include <mutex>

QPolygonF FloodFill::maskToPoly(QList<QPointF> mask)
{
    QPainterPath qpp;
    for (QPointF &p : mask)
        qpp.addRect(p.x(), p.y(), 1, 1);
    return qpp.simplified().toFillPolygon();
}

QList<QPointF> FloodFill::neighbors(QPointF &p)
{
    QList<QPointF> ret;
    int x = p.x();
    int y = p.y();
    int imgHeight = image.height()/scaleFactor;
    int imgWidth = image.width()/scaleFactor;

    switch (connectMode) {
    case C4:
        /* 4 Neighbor */
        if (y - 1 > 0        ) ret.push_back(QPointF(x    , y - 1));
        if (x + 1 < imgWidth ) ret.push_back(QPointF(x + 1, y    ));
        if (y + 1 < imgHeight) ret.push_back(QPointF(x    , y + 1));
        if (x - 1 > 0        ) ret.push_back(QPointF(x - 1, y    ));
        break;
    case C8:
        if (x - 1 > 0        && y - 1 > 0        ) ret.push_back(QPointF(x - 1, y - 1));
        if (                    y - 1 > 0        ) ret.push_back(QPointF(x    , y - 1));
        if (x + 1 < imgWidth && y - 1 > 0        ) ret.push_back(QPointF(x + 1, y - 1));
        if (x + 1 < imgWidth                     ) ret.push_back(QPointF(x + 1, y    ));
        if (x + 1 < imgWidth && y + 1 < imgHeight) ret.push_back(QPointF(x + 1, y + 1));
        if (                    y + 1 < imgHeight) ret.push_back(QPointF(x    , y + 1));
        if (x - 1 > 0        && y + 1 < imgHeight) ret.push_back(QPointF(x - 1, y + 1));
        if (x - 1 > 0                            ) ret.push_back(QPointF(x - 1, y    ));
        break;
    }

    return ret;
}

uint qHash(const QPointF &val) {
    return qHash(QPair<qreal, qreal>(val.x(), val.y()));
}

static std::mutex mtx;
QPolygonF FloodFill::compute(QPointF &p, int thresh) {
    mtx.lock();
    qDebug() << "in FloodFill::compute";
    QSet<QPointF> stack;
    QSet<QPointF> mask;
    QSet<QPointF> rejected;

    QTransform trans;
    trans = trans.scale(1/scaleFactor, 1/scaleFactor);
    QImage img = image.transformed(trans);

    stack.insert(p);
    qDebug() << "reference point" << p;

    QRgb colP = img.pixel(p.toPoint());
    int grayP = qGray(colP);

    int i = 0;
    while (!stack.empty()) {
        i++;
        if (i%1000 == 0) qDebug() << "iteration" << i << "stack" << stack.size();
        QPointF curr = *stack.begin();
        stack.remove(curr);
        if (stack.size() == 0) qDebug() << "stack empty";
        mask.insert(curr);

        for (QPointF &n : neighbors(curr)) {
            if (stack.contains(n) || (mask.find(n) != mask.end()) || (rejected.find(n) != rejected.end()))
                continue;

            QRgb colN = img.pixel(n.toPoint());
            int grayN = qGray(colN);
            int diff = abs(grayP - grayN);
            if (diff < thresh)
                stack.insert(n);
            else
                rejected.insert(n);
        }
    }

    QList<QPointF> l = mask.toList();
    std::sort(l.begin(), l.end(), [](const QPointF &a, const QPointF &b){ return (a.x() == b.x())?a.x() > b.x():a.y() > b.y(); });
    QPolygonF qp = maskToPoly(mask.toList());

    qDebug() << "polygon is:" << qp;
    mtx.unlock();
    return qp;
}
