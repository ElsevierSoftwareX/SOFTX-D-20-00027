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
#ifndef MERGE_H
#define MERGE_H

#include <QLineF>
#include <QPair>
#include <QPolygonF>

class Merge
{
public:
    Merge() = default;
    static QPolygonF compute(QPolygonF&, QPolygonF&);
    static QPolygonF segment(const QPolygonF &poly, const QPointF &start, const QPointF &end);

private:
    static QPair<QPointF,QPointF> tangentPoints(QPointF&, QPolygonF&);
    static int polygonRotation(QPolygonF& poly);
    static QList<QPair<QPointF, qreal>> angleToAll(QPointF &c, QPolygonF &poly);
    static QPair<QPointF, QPointF> extremaToBase(QList<QPair<QPointF, qreal>> points);
    static QList<QLineF> segmentsOf(QPolygonF &p);
};

#endif // MERGE_H
