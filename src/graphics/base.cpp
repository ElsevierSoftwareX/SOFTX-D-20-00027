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
#include "base.h"

#include <QDebug>

#include "provider/dataprovider.h"
#include "provider/guistate.h"

namespace CellTracker {

bool Base::cut(QPolygonF &objectPoly, QPolygonF &linePoly) {
    QPainterPath pp, pp2;
    pp.addPolygon(objectPoly);
    pp2.addPolygon(linePoly);
    return pp.intersects(pp2);
}

bool Base::cut(std::shared_ptr<Object> object, QPolygonF &linePoly) {
    return cut(*object->getOutline(), linePoly);
}

bool Base::pointInObject(qreal x, qreal y) {
    return DataProvider::getInstance()->cellAt(x, y) != nullptr;
}

bool Base::pointInObject(QPointF &&p) {
    return pointInObject(p.x(), p.y());
}

std::shared_ptr<CellTracker::Object> Base::objectCutByLine(QLineF &line) {
    bool p1obj = pointInObject(line.p1());
    bool p2obj = pointInObject(line.p2());

    if (p1obj) {
        qDebug() << "start point was inside object";
        return nullptr;
    }

    if (p2obj) {
        qDebug() << "end point was inside object";
        return nullptr;
    }

    QPolygonF linePoly;
    double sf = DataProvider::getInstance()->getScaleFactor();
    linePoly << line.p1()/sf << line.p2()/sf;
    QList<std::shared_ptr<Object>> cutObjects;

    int currFrame = GUIState::getInstance()->getCurrentFrame();
    std::shared_ptr<Frame> f = GUIState::getInstance()->getProj()->getMovie()->getFrame(currFrame);

    for (std::shared_ptr<Slice> s : f->getSlices()) {
        for (std::shared_ptr<Channel> c : s->getChannels().values()) {
            for (std::shared_ptr<Object> o : c->getObjects().values()) {
                if (cut(o, linePoly))
                    cutObjects.push_back(o);
            }
        }
    }

    if (cutObjects.count() == 0)
        return nullptr;

    if (cutObjects.count() > 2) {
        qDebug() << "more than one object was cut by the line";
        return nullptr;
    }

    return cutObjects.first();
}
}
