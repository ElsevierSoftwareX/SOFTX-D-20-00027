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
