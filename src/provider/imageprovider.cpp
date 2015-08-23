#include <QtDebug>
#include <QPainter>

#include "imageprovider.h"
#include "corrected_data/trackevent.h"
#include "corrected_data/trackeventdivision.h"
#include "src/provider/ctsettings.h"
#include "src/provider/dataprovider.h"
#include "src/provider/guistate.h"

using namespace CellTracker;

ImageProvider::ImageProvider() : QQuickImageProvider(Image) { }
ImageProvider::~ImageProvider() { }

bool ImageProvider::cellIsSelected(std::shared_ptr<Object> o) {
    std::shared_ptr<Object> selected = GUIState::getInstance()->getSelectedCell();

    return (selected
            && selected->getId() == o->getId()
            && selected->getFrameId() == o->getFrameId());
}

bool ImageProvider::cellAutoTrackletIsSelected(std::shared_ptr<Object> o) {
    std::shared_ptr<AutoTracklet> selected = GUIState::getInstance()->getSelectedAutoTrack();
    return (selected
            && selected->getID() >= 0
            && (uint32_t)selected->getID() == o->getAutoId());
}

bool ImageProvider::cellIsHovered(std::shared_ptr<Object> o) {
    std::shared_ptr<Object> hovered = GUIState::getInstance()->getHoveredCell();

    return (hovered
            && hovered->getId() == o->getId()
            && hovered->getFrameId() == o->getFrameId());
}

bool ImageProvider::cellIsInDaughters(std::shared_ptr<Object> daughter) {
    bool objInDaughters = false;

    std::shared_ptr<Tracklet> t = GUIState::getInstance()->getSelectedTrack();

    if(t && t->getNext() && t->getNext()->getType() == TrackEvent<Tracklet>::EVENT_TYPE_DIVISION) {
        std::shared_ptr<TrackEventDivision<Tracklet>> ev = std::static_pointer_cast<TrackEventDivision<Tracklet>>(t->getNext());
        for (std::shared_ptr<Tracklet> dt: *ev->getNext()) {
            objInDaughters |= dt->getStart().second == daughter;
        }
    }

    return objInDaughters;
}

bool ImageProvider::cellIsInTracklet(std::shared_ptr<Object> o) {
    return o->isInTracklet();
}

QColor ImageProvider::getCellLineColor(std::shared_ptr<Object> o) {
    QColor lineColor;

    if (cellIsSelected(o)) {
        lineColor = CTSettings::value("drawing/selected_linecolor").value<QColor>();
    } else {
        lineColor = CTSettings::value("drawing/unselected_linecolor").value<QColor>();
    }

    return lineColor;
}

qreal ImageProvider::getCellLineWidth(std::shared_ptr<Object> o) {
    qreal lineWidth;

    if (cellIsSelected(o)) {
        lineWidth = CTSettings::value("drawing/selected_linewidth").toReal();
    } else {
        lineWidth = CTSettings::value("drawing/default_linewidth").toReal();
    }

    return lineWidth;
}

Qt::BrushStyle ImageProvider::getCellBrushStyle(std::shared_ptr<Object> o, QPolygonF &outline, QPointF &mousePos)
{
    Q_UNUSED(o)
    Q_UNUSED(outline)
    Q_UNUSED(mousePos)

    Qt::BrushStyle style;
    /* maybe do something else here */
    style = Qt::SolidPattern;

    return style;
}

QColor ImageProvider::getCellBgColor(std::shared_ptr<Object> o)
{
    QColor bgColor;

    if (cellIsHovered(o))
        bgColor = CTSettings::value("drawing/active_cell").value<QColor>();
    else if (cellIsInDaughters(o))
        bgColor = CTSettings::value("drawing/merge_cell").value<QColor>();
    else if (cellIsInTracklet(o))
        bgColor = CTSettings::value("drawing/finished_cell").value<QColor>();
    else if (cellAutoTrackletIsSelected(o))
        bgColor = CTSettings::value("drawing/selected_track").value<QColor>();
    else
        bgColor = CTSettings::value("drawing/default_cell").value<QColor>();

    return bgColor;
}

void ImageProvider::drawPolygon(QPainter &painter, QPolygonF &poly, QColor col, Qt::BrushStyle style) {
    QBrush brush(col, style);
    brush.setColor(col);

    /* draws very nicely, but is slow */
    QPainterPath path;
    path.addPolygon(poly);
    painter.setOpacity(CTSettings::value("drawing/cell_opacity").toReal());
    painter.drawPath(path);
    painter.fillPath(path, brush);
}

void ImageProvider::drawOutlines(QImage &image, int frame, double scaleFactor) {
    /* set up painting equipment */
    QPainter painter(&image);

    if (!GUIState::getInstance()->getProj())
        return;

    /* collect the polygons we want to draw */
    QList<std::shared_ptr<Object>> allObjects;
    for (std::shared_ptr<Slice> s : GUIState::getInstance()->getProj()->getMovie()->getFrame(frame)->getSlices())
        for (std::shared_ptr<Channel> c : s->getChannels().values())
            allObjects.append(c->getObjects().values());

    for (std::shared_ptr<Object> o : allObjects) {
        QPolygonF curr;
        for (QPointF p : *(o->getOutline()))
            /* scale points to fit the image */
            curr.append(QPoint(p.x() * scaleFactor,
                               p.y() * scaleFactor));

        QPointF mousePos(GUIState::getInstance()->getMouseX(),
                         GUIState::getInstance()->getMouseY());

        QColor lineColor = getCellLineColor(o);
        qreal lineWidth = getCellLineWidth(o);

        QPen pen(lineColor, lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);

        QColor bgColor = getCellBgColor(o);
        Qt::BrushStyle bStyle = getCellBrushStyle(o, curr, mousePos);
        drawPolygon(painter, curr, bgColor, bStyle);

        /* draw the trackid */
        if (o && o->isInTracklet()) {
            QFont font = painter.font();
            font.setPointSize(CTSettings::value("text/trackid_fontsize").toInt());
            font.setBold(true);
            painter.setFont(font);
            pen = QPen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            painter.setPen(pen);
            painter.setOpacity(1);
            painter.drawText(o->getBoundingBox()->center() * scaleFactor,QString(std::to_string(o->getTrackId()).c_str()));
        }

//        QRect bb = *o->getBoundingBox();
//        QRect bb2(bb.topLeft()*scaleFactor, bb.bottomRight()*scaleFactor);
//        painter.drawRect(bb2);
    }

}

QImage ImageProvider::defaultImage(QSize *size, const QSize &requestedSize) {
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
QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(id);
    QImage newImage;

    int frame = GUIState::getInstance()->getCurrentFrame();
    QString path = GUIState::getInstance()->getProjPath();

    if (path.isEmpty() || frame < 0 || frame > GUIState::getInstance()->getMaximumFrame())
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

    /* draw the outlines over the given image if drawOutlines is enabled */
    if (GUIState::getInstance()->getDrawOutlines())
        drawOutlines(newImage, frame, scaleFactor);

    size->setHeight(newImage.height());
    size->setWidth(newImage.width());

    return newImage;
}
