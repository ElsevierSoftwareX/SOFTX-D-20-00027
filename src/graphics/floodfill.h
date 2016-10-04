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

    FloodFill(QImage &img, double sf, mode cm = C8) : image(img), scaleFactor(sf), connectMode(cm) {}
    ~FloodFill() = default;

    QPolygonF compute(QPoint &p, int thresh);

private:
    QPolygonF maskToPoly(QList<QPoint> mask);
    QPolygonF maskToPoly2(QList<QPoint> mask);
    QList<QPoint> neighbors(QPoint &p);

    QImage &image;
    double scaleFactor;
    mode connectMode;
};

#endif // FLOODFILL_H
