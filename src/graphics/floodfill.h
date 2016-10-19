#ifndef FLOODFILL_H
#define FLOODFILL_H

#include <QHash>
#include <QImage>
#include <QList>
#include <QPoint>
#include <QRect>
#include <QPair>
#include <QSet>

class FloodFill
{
public:
    enum mode {
        C4,
        C8,
    };

    FloodFill(QImage img, double sf, mode cm = C8) : image(img), scaleFactor(sf), connectMode(cm) {}
    ~FloodFill() = default;

    QPolygonF compute(QPoint p, int thresh);

    static QPolygonF maskToPoly(QList<QPoint> mask);
    static QPolygonF maskToPoly2(QList<QPoint> mask);
    static QPolygonF maskToPoly3(QList<QPoint> points);
    static QSet<QPoint> neighbors(QPoint &p, mode connectMode, QSize &imgSize);
    static QSet<QPoint> calculateMask(QImage &img, QPoint &p, int thresh, mode connectMode);

private:
    QImage image;
    double scaleFactor;
    mode connectMode;
};

#endif // FLOODFILL_H
