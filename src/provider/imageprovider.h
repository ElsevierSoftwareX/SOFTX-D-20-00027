/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2016, 2015 Enrico Uhlig, Sebastian Wagner
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
#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QColor>
#include <QImage>
#include <QObject>
#include <QString>
#include <QSize>
#include <QQuickImageProvider>

#include "io/importhdf5.h"

namespace CellTracker {
/*!
 * \brief The ImageProvider class
 *
 * This QQuickImageProvider is responsible for drawing the images displayed in CellTracker.
 * This includes getting them via ImportHDF5::requestImage() and then drawing the outlines
 * and Tracklet-Numbers of the cells over those images.
 *
 * It also implements a Caching-Mechanism to avoid re-requesting the image over and over
 * again when only the outlines should be drawn another way.
 */
class ImageProvider : public QQuickImageProvider
{
public:
    explicit ImageProvider();
    ~ImageProvider() = default;

    bool cellIsSelected(std::shared_ptr<Object> const &o);
    bool cellAutoTrackletIsSelected(std::shared_ptr<Object> const &o);
    bool cellIsHovered(std::shared_ptr<Object> const &o);
    bool cellIsInDaughters(std::shared_ptr<Object> const &daughter);
    bool cellIsInTracklet(std::shared_ptr<Object> const &o);
    bool cellIsRelated(std::shared_ptr<Object> const &o);

    QColor getCellLineColor(std::shared_ptr<Object> const &o);
    qreal getCellLineWidth(std::shared_ptr<Object> const &o);
    QColor getCellBgColor(std::shared_ptr<Object> const &o);
    Qt::BrushStyle getCellBrushStyle(std::shared_ptr<Object> const &o, QPolygonF const &outline, QPointF const &mousePos);

    QImage defaultImage(QSize *size, const QSize &requestedSize);

    void drawPolygon(QPainter &painter, QPolygon &poly, QColor col, Qt::BrushStyle style);
    void drawOutlines(QImage &image, int frame, int slice, int channel, double scaleFactor, bool regular, bool separation, bool aggregation, bool deletion, bool flood);
    void drawObjectInfo(QImage &image, int frame, int slice, int channel, double scaleFactor, bool drawTrackletIDs, bool drawAnnotationInfo);
    void drawCutLine(QImage &image);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
private:
    QImage cachedImage;
    QString cachedPath = "";
    int cachedFrame = -1;
    int cachedSlice = -1;
    int cachedChannel = -1;
};
}

#endif // IMAGEPROVIDER_H
