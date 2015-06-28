#include <QtDebug>
#include <QPainter>

#include "imageprovider2.h"
#include "src/provider/ctsettings.h"
#include "src/provider/dataprovider.h"
#include "src/provider/guistate.h"

using namespace CellTracker;
ImageProvider2::ImageProvider2() :
    QQuickImageProvider(Image)
{
    GUIState::getInstance()->setObjectID(-1);
    GUIState::getInstance()->setTrackID(-1);
    GUIState::getInstance()->setImageNumber(0);
    GUIState::getInstance()->setCurrentImage(-1);
    GUIState::getInstance()->setStrategyStep(1);
    GUIState::getInstance()->setSelectedCellID(-1);
    GUIState::getInstance()->setSelectedTrackID(-1);
    mouseArea = NULL;
    GUIState::getInstance()->setStatus("");
}

ImageProvider2::~ImageProvider2()
{
}

void ImageProvider2::setMotherCell()
{
    if(GUIState::getInstance()->getSelectedCell() != nullptr) {
        GUIState::getInstance()->setStrategyStep(2);
        GUIState::getInstance()->setMotherCell(GUIState::getInstance()->getSelectedCell());
        GUIState::getInstance()->getDaughterCells().clear();
        mouseArea->setProperty("status", "Select daughter objects - press space when finished");
    }
    else {
        mouseArea->setProperty("status", "Select mother track");
    }
}

void ImageProvider2::setDaughterCells()
{
    std::shared_ptr<CellTracker::Tracklet> mother = GUIState::getInstance()->getProj()->getGenealogy()->getTracklet(GUIState::getInstance()->getMotherCell()->getAutoId());
    for(int i = 0; i < GUIState::getInstance()->getDaughterCells().size(); ++i) {
        std::shared_ptr<CellTracker::Tracklet> daughter = GUIState::getInstance()->getProj()->getGenealogy()->getTracklet(GUIState::getInstance()->getDaughterCells().at(i)->getAutoId());
        GUIState::getInstance()->getProj()->getGenealogy()->addDaughterTrack(mother, daughter);
    }
    mouseArea->setProperty("status", "Daughter tracks added");
    GUIState::getInstance()->getDaughterCells().clear();
}

void ImageProvider2::setMouseArea(QObject *area)
{
    mouseArea = area;
}

QColor ImageProvider2::getCellColor(std::shared_ptr<CellTracker::Object> o, QPolygonF &outline, QPointF &mousePos)
{
    QColor color;

    bool mouseInShape = outline.containsPoint(mousePos, Qt::OddEvenFill);
    bool objInTracklet = o->isInTracklet();
    bool objInDaughters = GUIState::getInstance()->getDaughterCells().contains(o);

    if (mouseInShape)
        color = CTSettings::value("tracking/display/cell/active").value<QColor>();
    else if (objInTracklet)
        color = CTSettings::value("tracking/display/cell/finished").value<QColor>();
    else if (objInDaughters)
        color = CTSettings::value("tracking/display/cell/merge").value<QColor>();
    else
        color = CTSettings::value("tracking/display/cell/default").value<QColor>();

    return color;
}

void ImageProvider2::drawPolygon(QPainter &painter, QPolygonF &poly, QColor col) {
    QBrush brush(col, Qt::SolidPattern);
    brush.setColor(col);

    QPainterPath path;
    path.addPolygon(poly);
    painter.setOpacity(CTSettings::value("tracking/display/cell/opacity").toReal());
    painter.drawPath(path);
    painter.fillPath(path, brush);
}

void ImageProvider2::drawOutlines(QImage &image, int frame, double scaleFactor) {
    /* set up painting equipment */
    QPainter painter(&image);
    QPen pen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);

    /* collect the polygons we want to draw */
    QList<std::shared_ptr<CellTracker::Object>> allObjects;
    for (std::shared_ptr<CellTracker::Slice> s : GUIState::getInstance()->getProj()->getMovie()->getFrame(frame)->getSlices())
        allObjects.append(s->getObjects().values());

    for (std::shared_ptr<CellTracker::Object> o : allObjects) {
        QPolygonF curr;
        QPoint tl = o->getBoundingBox()->topLeft();
        for (QPointF p : *(o->getOutline())) {
            /* mirror the polygon */
            double x = tl.x() - tl.y() + p.y();
            double y = tl.y() - tl.x() + p.x();

            /* scale points to fit the image */
            x *= scaleFactor;
            y *= scaleFactor;
            curr.append(QPoint(x,y));
        }

        QPointF mousePos(mouseArea->property("lastX").toFloat(),
                         mouseArea->property("lastY").toFloat());

        QColor color = getCellColor(o, curr, mousePos);
        drawPolygon(painter, curr, color);
    }

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

    /* Get the image path and the current slider value. */
    if(mouseArea) {
        GUIState::getInstance()->setPath(mouseArea->property("path").toString());
        GUIState::getInstance()->setImageNumber(mouseArea->property("sliderValue").toInt() - 1);
    }

    DataProvider dataProvider;
    QImage newImage = dataProvider.requestImage(
                GUIState::getInstance()->getPath(),
                GUIState::getInstance()->getImageNumber());

    if (!requestedSize.isValid())
        return newImage;

    double oldWidth = newImage.width();
    newImage = newImage.scaled(requestedSize,Qt::KeepAspectRatio);
    double newWidth = newImage.width();
    double scaleFactor = newWidth/oldWidth;

    /* draw the outlines over the given image */
    drawOutlines(newImage, GUIState::getInstance()->getImageNumber(), scaleFactor);

    size->setHeight(newImage.height());
    size->setWidth(newImage.width());

    return newImage;
}
