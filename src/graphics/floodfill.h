/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2016 Sebastian Wagner
 *
 * Celltracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Celltracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Celltracker.  If not, see <https://www.gnu.org/licenses/>.
 */
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

    static QPolygonF maskToPoly(QList<QPoint> points);
    static QSet<QPoint> neighbors(QPoint &p, mode connectMode, QSize &imgSize);
    static QSet<QPoint> calculateMask(QImage &img, QPoint &p, int thresh, int maxPxls, mode connectMode);

private:
    QImage image;
    double scaleFactor;
    mode connectMode;
};

#endif // FLOODFILL_H
