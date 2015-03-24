#include <QtDebug>
#include <QPainter>

#include "imageprovider.h"
#include "src/provider/importobject.h"

ImageProvider::ImageProvider() :
    QQuickImageProvider(Image)
{
    objectID = -1;
    trackID = -1;
    imageNumber = 0;
    currentImage = -1;
    lastObjectID = -1;
    selectedCell = -1;
    mouseArea = NULL;
    status = "";
}

ImageProvider::~ImageProvider()
{
}

/*!
 * \brief Returns the object ID of the current cell.
 * \return object ID
 */
int ImageProvider::getObjectID()
{
    return objectID;
}

/*!
 * \brief Returns the track ID of the current cell.
 * \return track ID
 */
int ImageProvider::getTrackID()
{
    return trackID;
}

/*!
 * \brief Returns the current entry of the status bar.
 * \return status entry
 */
QString ImageProvider::getStatus()
{
    return status;
}

void ImageProvider::setMouseArea(QObject *area)
{
    mouseArea = area;
}

void ImageProvider::setLastObjectID(int id)
{
    lastObjectID = id;
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
    /* Get the image path and the current slider value. */
    if(mouseArea) {
        path = mouseArea->property("path").toString();
        imageNumber = mouseArea->property("sliderValue").toInt() - 1;
    }

    ImportObject MyImport;
    newImage = MyImport.requestImage(path, imageNumber);
    QPainter painter(&newImage);

    QPen pen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);

    int action;
    int strategy = 0;
    QPoint mousePosition;

    /* Get the mouse action and the position of the cursor. */
    if(mouseArea) {
        if(mouseArea->property("mouseAction") == "leftClick")
            action = 1;
        else if(mouseArea->property("mouseAction") == "hover")
            action = 2;
        mousePosition.setX(mouseArea->property("lastX").toInt());
        mousePosition.setY(mouseArea->property("lastY").toInt());
    }

    /* Get the strategy. */
    if(mouseArea) {
        if(mouseArea->property("strategy") == "combine tracklets")
            strategy = 1;
    }

    /* If you are still in the same frame, the selected cell is painted red. */
    if(imageNumber != currentImage)
        currentImage = -1;

    bool cellHasBeenSelected = false;
    bool cellHasBeenHovered = false;

    /* Iterate through all cells of the current frame. */
    for(int i = 0; i < listOfImages.at(imageNumber).size(); ++i) {
        QPolygon polygon;
        std::shared_ptr<CellTracker::Object> object = listOfImages.at(imageNumber).at(i);

        /* Draw the outlines of the cell. */
        std::shared_ptr<QRect> rect = object->getBoundingBox();
        for(QPointF point: object->getOutline()->toStdVector()) {
            //polygon << QPoint(point.x(), point.y());
            int x = rect->topLeft().x() - rect->topLeft().y() + point.y();
            int y = rect->topLeft().y() - rect->topLeft().x() + point.x();
            polygon << QPoint(x, y);
        }
        painter.drawPolygon(polygon);

        int newColor = 0;
        int oldColor = listOfImageColors.at(imageNumber).at(i);

        /* If a cell has been selected, save its ID and frame number. */
        if(polygon.containsPoint(mousePosition, Qt::OddEvenFill) && action == 1) {
            cellHasBeenSelected = true;
            newColor = 3;
            selectedCell = object->getID();
            currentImage = imageNumber;

            /* If tracklets shall be combined, remember the first cell and
               jump to the last frame of its tracklet. Then wait until the
               second cell has been selected. */
            if(strategy == 1) {
                if(lastObjectID == -1) {
                    lastObjectID = object->getID();
                    mouseArea->setProperty("jumpFrames", 1);
                    mouseArea->setProperty("status", "Select following cell object");
                }
                else {
                    lastObjectID = -1;
                    mouseArea->setProperty("status", "Tracklets combined - Select cell object");
                }
            }
            else
                status = "";
        }

        /* If you hovered over a cell, get its object and track ID. */
        else if(polygon.containsPoint(mousePosition, Qt::OddEvenFill) && action == 2) {
            cellHasBeenHovered = true;
            newColor = 3;
            objectID = object->getID();
            //trackID = object->getTrackID();
        }

        /* The selected cell is painted red in the current frame. */
        else if(object->getID() == selectedCell && currentImage == imageNumber)
            newColor = 3;
        /* The selected cell is painted yellow in other frames. */
        else if(object->getID() == selectedCell)
            newColor = 2;
        else
            newColor = 0;
        listOfImageColors[imageNumber].replace(i, newColor);

        QBrush brush;
        QPainterPath path;
        brush.setStyle(Qt::SolidPattern);

        /* Paint the current cell. */
        if(newColor == 0)
            brush.setStyle(Qt::NoBrush);
        else if(newColor == 1)
            brush.setColor(Qt::green);
        else if(newColor == 2)
            brush.setColor(Qt::yellow);
        else if(newColor == 3)
            brush.setColor(Qt::red);

        path.addPolygon(polygon);
        painter.fillPath(path, brush);
    }

    /* If no cell has been hovered or selected, delete the current ID. */
    if(selectedCell == -1 && !cellHasBeenHovered && !cellHasBeenSelected) {
        objectID = -1;
    }
    /* In case of clicking between cells, delete the selected cell. */
    else if(action == 1 && !cellHasBeenSelected) {
        selectedCell = -1;
    }
    /* In case of hovering between cells, show the ID of the selected cell. */
    else if(!cellHasBeenHovered) {
        objectID = selectedCell;
    }

    return newImage;
}
