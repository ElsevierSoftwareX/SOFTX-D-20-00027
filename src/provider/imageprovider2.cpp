#include <QtDebug>
#include <QPainter>

#include "imageprovider2.h"
#include "src/provider/ctsettings.h"
#include "src/provider/dataprovider.h"

using namespace CellTracker;
ImageProvider2::ImageProvider2() :
    QQuickImageProvider(Image)
{
    objectID = -1;
    trackID = -1;
    imageNumber = 0;
    currentImage = -1;
    strategyStep = 1;
    selectedCellID = -1;
    selectedTrackID = -1;
    mouseArea = NULL;
    status = "";
}

ImageProvider2::~ImageProvider2()
{
}

/*!
 * \brief Returns the object ID of the current cell.
 * \return object ID
 */
int ImageProvider2::getObjectID()
{
    return objectID;
}

/*!
 * \brief Returns the track ID of the current cell.
 * \return track ID
 */
int ImageProvider2::getTrackID()
{
    return trackID;
}

/*!
 * \brief Returns the object ID of the selected cell.
 * \return selected cell ID
 */
int ImageProvider2::getSelectedCellID()
{
    return selectedCellID;
}

/*!
 * \brief Returns the frame number of the current cell.
 * \return image number
 */
int ImageProvider2::getImageNumber()
{
    return imageNumber;
}

/*!
 * \brief Returns the frame number of the selected cell.
 * \return image number
 */
int ImageProvider2::getCurrentImage()
{
    return currentImage;
}

/*!
 * \brief Returns the current entry of the status bar.
 * \return status entry
 */
QString ImageProvider2::getStatus()
{
    return status;
}

void ImageProvider2::setMotherCell()
{
    if(selectedCell != nullptr) {
        strategyStep = 2;
        motherCell = selectedCell;
        daughterCells.clear();
        mouseArea->setProperty("status", "Select daughter objects - press space when finished");
    }
    else {
        mouseArea->setProperty("status", "Select mother track");
    }
}

void ImageProvider2::setDaughterCells()
{
    std::shared_ptr<CellTracker::Tracklet> mother = proj->getGenealogy()->getTracklet(motherCell->getAutoId());
    for(int i = 0; i < daughterCells.size(); ++i) {
        std::shared_ptr<CellTracker::Tracklet> daughter = proj->getGenealogy()->getTracklet(daughterCells.at(i)->getAutoId());
        proj->getGenealogy()->addDaughterTrack(mother, daughter);
    }
    mouseArea->setProperty("status", "Daughter tracks added");
    daughterCells.clear();
}

void ImageProvider2::setMouseArea(QObject *area)
{
    mouseArea = area;
}

void ImageProvider2::setStrategyStep(int step)
{
    strategyStep = step;
}

void ImageProvider2::setProject(std::shared_ptr<CellTracker::Project> proj)
{
    this->proj = proj;
}

QColor ImageProvider2::getCellColor(std::shared_ptr<CellTracker::Object> o, QPolygonF &outline, QPointF &mousePos)
{
    QColor color;

    bool mouseInShape = outline.containsPoint(mousePos, Qt::OddEvenFill);
    bool objInTracklet = o->isInTracklet();
    bool objInDaughters = daughterCells.contains(o);

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
    for (std::shared_ptr<CellTracker::Slice> s : proj->getMovie()->getFrame(frame)->getSlices())
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
        path = mouseArea->property("path").toString();
        imageNumber = mouseArea->property("sliderValue").toInt() - 1;
    }

    DataProvider dataProvider;
    newImage = dataProvider.requestImage(path, imageNumber);

    if (!requestedSize.isValid())
        return newImage;

    double oldWidth = newImage.width();
    newImage = newImage.scaled(requestedSize,Qt::KeepAspectRatio);
    double newWidth = newImage.width();
    double scaleFactor = newWidth/oldWidth;

    /* draw the outlines over the given image */
    drawOutlines(newImage, imageNumber, scaleFactor);

    qDebug() << newImage.size();
    size->setHeight(newImage.height());
    size->setWidth(newImage.width());

    return newImage;
}

/*!
 * \brief Returns the current cell.
 * \return current cell object
 */
std::shared_ptr<CellTracker::Object> ImageProvider2::getCurrentCell()
{
    return currentCell;
}

/*!
 * \brief Returns the selected cell.
 * \return selected cell object
 */
std::shared_ptr<CellTracker::Object> ImageProvider2::getSelectedCell()
{
    return selectedCell;
}
