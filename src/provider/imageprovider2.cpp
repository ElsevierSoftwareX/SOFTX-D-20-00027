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

QColor ImageProvider2::getCellLineColor(std::shared_ptr<Object> o) {
    QColor lineColor;
    std::shared_ptr<Object> selected = GUIState::getInstance()->getNewSelectedCell();

    /*! \todo from config */
    if (selected
            && selected->getId() == o->getId()
            && selected->getFrameId() == o->getFrameId()) {
        lineColor = CTSettings::value("colors/selected_linecolor").value<QColor>();
    } else {
        lineColor = CTSettings::value("colors/unselected_linecolor").value<QColor>();
    }

    return lineColor;
}

QColor ImageProvider2::getCellBgColor(std::shared_ptr<Object> o, QPolygonF &outline, QPointF &mousePos)
{
    QColor bgColor;

    bool mouseInShape = outline.containsPoint(mousePos, Qt::OddEvenFill);
    bool objInTracklet = o->isInTracklet();
    std::shared_ptr<Tracklet> t = GUIState::getInstance()->getNewSelectedTrack();

    bool objInDaughters = false;
    if(t && t->getNext() && t->getNext()->getType() == TrackEvent<Tracklet>::EVENT_TYPE_DIVISION) {
        std::shared_ptr<TrackEventDivision<Tracklet>> ev = std::static_pointer_cast<TrackEventDivision<Tracklet>>(t->getNext());
        for (std::shared_ptr<Tracklet> dt: *ev->getNext()) {
            objInDaughters |= dt->getStart().second == o;
        }
    }

    if (mouseInShape)
        bgColor = CTSettings::value("colors/active_cell").value<QColor>();
    else if (objInTracklet)
        bgColor = CTSettings::value("colors/finished_cell").value<QColor>();
    else if (objInDaughters)
        bgColor = CTSettings::value("colors/merge_cell").value<QColor>();
    else
        bgColor = CTSettings::value("colors/default_cell").value<QColor>();

    return bgColor;
}

void ImageProvider2::drawPolygon(QPainter &painter, QPolygonF &poly, QColor col) {
    QBrush brush(col, Qt::SolidPattern);
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
        drawPolygon(painter, curr, bgColor);

        /* draw the cellid */
        painter.drawText(o->getBoundingBox()->center() * scaleFactor,QString(std::to_string(o->getId()).c_str()));
    }

}

QImage ImageProvider2::defaultImage(QSize *size, const QSize &requestedSize) {
    QImage defaultImage(requestedSize.width(),requestedSize.height(),QImage::Format_RGB32);
    defaultImage.fill(Qt::black);
    size->setHeight(defaultImage.height());
    size->setWidth(defaultImage.width());
    QPainter painter(&defaultImage);

    int w = defaultImage.width(), h = defaultImage.height();
    painter.setFont(QFont("DejaVu Serif", 64));
    painter.setPen(QPen(Qt::green));
    painter.drawText(QRect(0,0,w,h),"CellTracker", QTextOption(Qt::AlignHCenter|Qt::AlignVCenter));

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

    newImage = DataProvider::getInstance()->requestImage(path, frame);

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
