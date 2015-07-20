#include <QtDebug>
#include <QPainter>

#include "imageprovider2.h"
#include "corrected_data/trackevent.h"
#include "corrected_data/trackeventdivision.h"
#include "src/provider/ctsettings.h"
#include "src/provider/dataprovider.h"
#include "src/provider/guistate.h"

using namespace CellTracker;

ImageProvider2::ImageProvider2() : QQuickImageProvider(Image) { }
ImageProvider2::~ImageProvider2() { }

bool ImageProvider2::cellIsSelected(std::shared_ptr<Object> o) {
    std::shared_ptr<Object> selected = GUIState::getInstance()->getNewSelectedCell();

    return (selected
            && selected->getId() == o->getId()
            && selected->getFrameId() == o->getFrameId());
}

bool ImageProvider2::cellAutoTrackletIsSelected(std::shared_ptr<Object> o) {
    std::shared_ptr<AutoTracklet> selected = GUIState::getInstance()->getNewSelectedAutoTrack();
    return (selected
            && selected->getID() >= 0
            && (uint32_t)selected->getID() == o->getAutoId());
}

bool ImageProvider2::cellIsHovered(QPolygonF &outline, QPointF &mousePos) {
    return outline.containsPoint(mousePos, Qt::OddEvenFill);
}

bool ImageProvider2::cellIsInDaughters(std::shared_ptr<Object> daughter) {
    bool objInDaughters = false;

    std::shared_ptr<Tracklet> t = GUIState::getInstance()->getNewSelectedTrack();

    if(t && t->getNext() && t->getNext()->getType() == TrackEvent<Tracklet>::EVENT_TYPE_DIVISION) {
        std::shared_ptr<TrackEventDivision<Tracklet>> ev = std::static_pointer_cast<TrackEventDivision<Tracklet>>(t->getNext());
        for (std::shared_ptr<Tracklet> dt: *ev->getNext()) {
            objInDaughters |= dt->getStart().second == daughter;
        }
    }

    return objInDaughters;
}

bool ImageProvider2::cellIsInTracklet(std::shared_ptr<Object> o) {
    return o->isInTracklet();
}

QColor ImageProvider2::getCellLineColor(std::shared_ptr<Object> o) {
    QColor lineColor;

    if (cellAutoTrackletIsSelected(o)) {
        lineColor = CTSettings::value("colors/selected_linecolor").value<QColor>();
    } else {
        lineColor = CTSettings::value("colors/unselected_linecolor").value<QColor>();
    }

    return lineColor;
}

Qt::BrushStyle ImageProvider2::getCellBrushStyle(std::shared_ptr<Object> o, QPolygonF &outline, QPointF &mousePos)
{
    Q_UNUSED(o)
    Q_UNUSED(outline)
    Q_UNUSED(mousePos)

    Qt::BrushStyle style;
    /* maybe do something else here */
    style = Qt::SolidPattern;

    return style;
}

QColor ImageProvider2::getCellBgColor(std::shared_ptr<Object> o, QPolygonF &outline, QPointF &mousePos)
{
    QColor bgColor;

    if (cellIsHovered(outline, mousePos))
        bgColor = CTSettings::value("colors/active_cell").value<QColor>();
    else if (cellIsSelected(o))
        bgColor = CTSettings::value("colors/selected_cell").value<QColor>();
    else if (cellIsInDaughters(o))
        bgColor = CTSettings::value("colors/merge_cell").value<QColor>();
    else if (cellIsInTracklet(o))
        bgColor = CTSettings::value("colors/finished_cell").value<QColor>();
    else if (cellAutoTrackletIsSelected(o))
        bgColor = CTSettings::value("colors/selected_track").value<QColor>();
    else
        bgColor = CTSettings::value("colors/default_cell").value<QColor>();

    return bgColor;
}

void ImageProvider2::drawPolygon(QPainter &painter, QPolygonF &poly, QColor col, Qt::BrushStyle style) {
    QBrush brush(col, style);
    brush.setColor(col);

    QPainterPath path;
    path.addPolygon(poly);
    painter.setOpacity(CTSettings::value("colors/cell_opacity").toReal());
    painter.drawPath(path);
    painter.fillPath(path, brush);
}

void ImageProvider2::drawOutlines(QImage &image, int frame, double scaleFactor) {
    /* set up painting equipment */
    QPainter painter(&image);

    if (!GUIState::getInstance()->getNewProj())
        return;

    /* collect the polygons we want to draw */
    QList<std::shared_ptr<Object>> allObjects;
    for (std::shared_ptr<Slice> s : GUIState::getInstance()->getNewProj()->getMovie()->getFrame(frame)->getSlices())
        allObjects.append(s->getObjects().values());

    for (std::shared_ptr<Object> o : allObjects) {
        QPolygonF curr;
        for (QPointF p : *(o->getOutline()))
            /* scale points to fit the image */
            curr.append(QPoint(p.x() * scaleFactor,
                               p.y() * scaleFactor));

        QPointF mousePos(GUIState::getInstance()->getNewMouseX(),
                         GUIState::getInstance()->getNewMouseY());

        QColor lineColor = getCellLineColor(o);
        QPen pen(lineColor, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);

        QColor bgColor = getCellBgColor(o, curr, mousePos);
        Qt::BrushStyle bStyle = getCellBrushStyle(o, curr, mousePos);
        drawPolygon(painter, curr, bgColor, bStyle);

        /* draw the cellid */
//        pen = QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
//        painter.setPen(pen);
//        painter.drawText(o->getBoundingBox()->center() * scaleFactor,QString(std::to_string(o->getId()).c_str()));
    }

}

QImage ImageProvider2::defaultImage(QSize *size, const QSize &requestedSize) {
    QImage defaultImage(requestedSize.width(),requestedSize.height(),QImage::Format_RGB32);
    defaultImage.fill(Qt::white);
    size->setHeight(defaultImage.height());
    size->setWidth(defaultImage.width());
    QPainter painter(&defaultImage);

    int w = defaultImage.width(), h = defaultImage.height();
    painter.setFont(QFont("DejaVu Serif", 64));
    painter.setPen(QPen(QColor(63,191,0)));
    painter.drawText(QRect(0,0,w,h),"CellTracker™", QTextOption(Qt::AlignHCenter|Qt::AlignVCenter));

    return defaultImage;
}


/*!
 * \brief Loads an image and draws the outlines of the cells.
 * \param id is an unused variable
 * \param size is an unused variable
 * \param requestedSize is an unused variable
 * \return a new QImage
 *
 * Drawing the cells is done in different steps:
 *   - Iterate through all cells of the current frame.
 *   - Draw the outlines of the cell.
 *   - Paint the current cell.
 *
 * Object and track ID are shown if you hovered over a cell.
 */
QImage ImageProvider2::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(id);
    QImage newImage;

    int frame = GUIState::getInstance()->getNewCurrentFrame();
    QString path = GUIState::getInstance()->getNewProjPath();

    if (path.isEmpty() || frame < 0 || frame > GUIState::getInstance()->getNewMaximumFrame())
        return defaultImage(size, requestedSize);

    /* some caching, so we don't always re-request the image */
    if (frame == cachedFrame && path == cachedPath) {
        newImage = cachedImage;
    } else {
        newImage = DataProvider::getInstance()->requestImage(path, frame);
        cachedImage = newImage;
        cachedPath = path;
        cachedFrame = frame;
    }

    if (!requestedSize.isValid())
        return newImage;

    double oldWidth = newImage.width();
    newImage = newImage.scaled(requestedSize,Qt::KeepAspectRatio);
    double newWidth = newImage.width();
    double scaleFactor = newWidth/oldWidth;
    DataProvider::getInstance()->setScaleFactor(scaleFactor);

    /* draw the outlines over the given image */
    drawOutlines(newImage, frame, scaleFactor);

    size->setHeight(newImage.height());
    size->setWidth(newImage.width());

    return newImage;
}
