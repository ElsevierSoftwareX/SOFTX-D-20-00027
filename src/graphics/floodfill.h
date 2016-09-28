#ifndef FLOODFILL_H
#define FLOODFILL_H

#include <QHash>
#include <QImage>
#include <QList>
#include <QPoint>
#include <QRect>
#include <QPair>

class FloodFill
{
public:
    enum mode {
        C4,
        C8,
    };

    FloodFill(QImage &img, double sf, mode cm = C4) : image(img), scaleFactor(sf), connectMode(cm) {}
    ~FloodFill() = default;

    QPolygonF compute(QPointF &p, int thresh);

private:
    QPolygonF maskToPoly(QList<QPointF> mask);
    QList<QPointF> neighbors(QPointF &p);

    QImage &image;
    double scaleFactor;
    mode connectMode;
};

#endif // FLOODFILL_H
