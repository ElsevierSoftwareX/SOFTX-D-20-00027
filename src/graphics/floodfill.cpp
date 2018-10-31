/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2017, 2016 Sebastian Wagner
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
#include "floodfill.h"

#include <QPolygonF>
#include <QList>
#include <QPoint>
#include <QPainterPath>
#include <QSet>
#include <QImage>

#include "provider/ctsettings.h"

/* Code adapted from Konstantin Thierbach's code */
QPolygonF FloodFill::maskToPoly(QList<QPoint> points)
{
    QPolygon outline;
    QSet<QPoint> set = points.toSet();
    QList<QPoint> directions = {
        QPoint{-1,0}, QPoint{-1,-1}, QPoint{0,-1}, QPoint{1,-1},
        QPoint{1,0}, QPoint{1,1}, QPoint{0,1}, QPoint{-1,1}
    };

    std::sort(points.begin(), points.end(),
              [](QPoint &a, QPoint &b){return (a.y() == b.y())?(a.x() < b.x()):(a.y() < b.y());});

    if (!points.empty()) {
        if(points.size() == 1) {
            outline.append(points.front());
            return outline;
        }
        QPoint b, c, b0, b1, tmp, c1, previousDir, currentDir;

        int currDir = directions.indexOf(QPoint(-1,0));

        b0 = points.at(0);

        for (int i = 1; i < directions.size(); i++) {
            previousDir = directions[currDir];
            currDir = (currDir + 1) % directions.size();
            currentDir = directions[currDir];
            tmp = b0 + currentDir;

            if (set.contains(tmp)) {
                b1 = tmp;
                c1 = b0 + previousDir;
                outline.append(b0);
                outline.append(b1);
                break;
            }
        }

        b = b1;
        c = c1;

        while (true) {
            currDir = directions.indexOf(c - b);

            for (int i = 1; i < directions.size(); i++) {
                previousDir = directions[currDir];
                currDir = (currDir + 1) % directions.size();
                currentDir = directions[currDir];
                tmp = b + currentDir;

                if (set.contains(tmp)) {
                    c = b + previousDir;
                    b = tmp;
                    outline.append(b);

                    break;
                }
            }
            if (b == b0)
                break;
        }
    }

    return outline;
}


QSet<QPoint> FloodFill::neighbors(QPoint &p, mode connectMode, QSize &imgSize)
{
    QSet<QPoint> ret;
    int x = p.x();
    int y = p.y();
    int imgHeight = imgSize.height();
    int imgWidth = imgSize.width();

    switch (connectMode) {
    case C4:
        /* 4 Neighbor */
        if (y - 1 > 0        ) ret.insert(QPoint(x    , y - 1));
        if (x + 1 < imgWidth ) ret.insert(QPoint(x + 1, y    ));
        if (y + 1 < imgHeight) ret.insert(QPoint(x    , y + 1));
        if (x - 1 > 0        ) ret.insert(QPoint(x - 1, y    ));
        break;
    case C8:
        /* 8 Neighbor */
        if (x - 1 > 0        && y - 1 > 0        ) ret.insert(QPoint(x - 1, y - 1));
        if (                    y - 1 > 0        ) ret.insert(QPoint(x    , y - 1));
        if (x + 1 < imgWidth && y - 1 > 0        ) ret.insert(QPoint(x + 1, y - 1));
        if (x + 1 < imgWidth                     ) ret.insert(QPoint(x + 1, y    ));
        if (x + 1 < imgWidth && y + 1 < imgHeight) ret.insert(QPoint(x + 1, y + 1));
        if (                    y + 1 < imgHeight) ret.insert(QPoint(x    , y + 1));
        if (x - 1 > 0        && y + 1 < imgHeight) ret.insert(QPoint(x - 1, y + 1));
        if (x - 1 > 0                            ) ret.insert(QPoint(x - 1, y    ));
        break;
    }

    return ret;
}

static uint qHash(const QPoint &val) {
    return qHash(QPair<int, int>(val.x(), val.y()));
}

QSet<QPoint> FloodFill::calculateMask(QImage &img, QPoint &p, int thresh, int maxPxls, mode connectMode)
{
    QSize size = img.size();
    QSet<QPoint> stack;
    QSet<QPoint> mask;
    QSet<QPoint> rejected;

    stack.insert(p);

    QRgb colP = img.pixel(p);
    int grayP = qGray(colP);

    int i = 0;
    while (!stack.empty()) {
        i++;
        QPoint curr = *stack.begin();
        stack.remove(curr);
        mask.insert(curr);

        for (const QPoint &n : neighbors(curr, connectMode, size)) {
            if (mask.contains(n))
                continue;

            QRgb colN = img.pixel(n);
            int grayN = qGray(colN);
            int diff = abs(grayP - grayN);
            if (diff < thresh)
                stack.insert(n);
            else
                rejected.insert(n);
        }

        if (mask.size() > maxPxls)
            break; /* stop this, if the mask gets to big */
    }

    return mask;
}

QPolygonF FloodFill::compute(QPoint p, int thresh) {
    QSet<QPoint> mask;
    QPolygonF outline;

    QTransform trans;
    trans = trans.scale(1/scaleFactor, 1/scaleFactor);
    QImage img = image.transformed(trans);

    int totalPxls = img.height() * img.width();
    qreal perc = CellTracker::CTSettings::value("graphics/max_pixelmask_percentage").toReal();
    int maxPxls = static_cast<int>(totalPxls * perc);

    mask = calculateMask(img, p, thresh, maxPxls, connectMode);
    outline = maskToPoly(mask.toList());

    return outline;
}
