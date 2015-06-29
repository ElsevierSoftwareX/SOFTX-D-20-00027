#include <QtDebug>
#include <QPainter>

#include "imageprovider2.h"
#include "src/provider/ctsettings.h"
#include "src/provider/dataprovider.h"
#include "src/provider/guistate.h"

using namespace CellTracker;

ImageProvider2::ImageProvider2() : QQuickImageProvider(Image) { }
ImageProvider2::~ImageProvider2() { }

void ImageProvider2::setMotherCell()
{
    if(GUIState::getInstance()->getSelectedCell() != nullptr) {
        GUIState::getInstance()->setStrategyStep(2);
        GUIState::getInstance()->setMotherCell(GUIState::getInstance()->getSelectedCell());
        GUIState::getInstance()->getDaughterCells().clear();
        GUIState::getInstance()->setStatus("Select daughter objects - press space when finished");
    }
    else {
        GUIState::getInstance()->setStatus("Select mother track");
    }
}

void ImageProvider2::setDaughterCells()
{
    std::shared_ptr<CellTracker::Tracklet> mother = DataProvider::getInstance()->getProj()->getGenealogy()->getTracklet(GUIState::getInstance()->getMotherCell()->getAutoId());
    for(int i = 0; i < GUIState::getInstance()->getDaughterCells().size(); ++i) {
        std::shared_ptr<CellTracker::Tracklet> daughter = DataProvider::getInstance()->getProj()->getGenealogy()->getTracklet(GUIState::getInstance()->getDaughterCells().at(i)->getAutoId());
        DataProvider::getInstance()->getProj()->getGenealogy()->addDaughterTrack(mother, daughter);
    }
    GUIState::getInstance()->setStatus("Daughter tracks added");
    GUIState::getInstance()->getDaughterCells().clear();
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

    if (!DataProvider::getInstance()->getProj())
        return;

    /* collect the polygons we want to draw */
    QList<std::shared_ptr<CellTracker::Object>> allObjects;
    for (std::shared_ptr<CellTracker::Slice> s : DataProvider::getInstance()->getProj()->getMovie()->getFrame(frame)->getSlices())
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

        QPointF mousePos(GUIState::getInstance()->getLastX(),
                         GUIState::getInstance()->getLastY());

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

    int frame = GUIState::getInstance()->getSliderValue();
    QString path = GUIState::getInstance()->getPath();

    GUIState::getInstance()->setCurrentFrame(frame);
    QImage newImage = DataProvider::getInstance()->requestImage(path, frame);

    if (!requestedSize.isValid())
        return newImage;

    double oldWidth = newImage.width();
    newImage = newImage.scaled(requestedSize,Qt::KeepAspectRatio);
    double newWidth = newImage.width();
    double scaleFactor = newWidth/oldWidth;

    /* draw the outlines over the given image */
    drawOutlines(newImage, GUIState::getInstance()->getCurrentFrame(), scaleFactor);

    size->setHeight(newImage.height());
    size->setWidth(newImage.width());

    return newImage;
}
