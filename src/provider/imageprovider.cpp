#include <QtDebug>
#include <QPainter>

#include "imageprovider.h"
#include "src/provider/dataprovider.h"

ImageProvider::ImageProvider() :
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
 * \brief Returns the object ID of the selected cell.
 * \return selected cell ID
 */
int ImageProvider::getSelectedCellID()
{
    return selectedCellID;
}

/*!
 * \brief Returns the frame number of the current cell.
 * \return image number
 */
int ImageProvider::getImageNumber()
{
    return imageNumber;
}

/*!
 * \brief Returns the frame number of the selected cell.
 * \return image number
 */
int ImageProvider::getCurrentImage()
{
    return currentImage;
}

/*!
 * \brief Returns the current entry of the status bar.
 * \return status entry
 */
QString ImageProvider::getStatus()
{
    return status;
}

void ImageProvider::setMotherCell()
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

void ImageProvider::setDaughterCells()
{
    std::shared_ptr<CellTracker::Tracklet> mother = proj->getGenealogy()->getTracklet(motherCell->getAutoId());
    for(int i = 0; i < daughterCells.size(); ++i) {
        std::shared_ptr<CellTracker::Tracklet> daughter = proj->getGenealogy()->getTracklet(daughterCells.at(i)->getAutoId());
        proj->getGenealogy()->addDaughterTrack(mother, daughter);
    }
    mouseArea->setProperty("status", "Daughter tracks added");
    daughterCells.clear();
}

void ImageProvider::setMouseArea(QObject *area)
{
    mouseArea = area;
}

void ImageProvider::setStrategyStep(int step)
{
    strategyStep = step;
}

void ImageProvider::setProject(std::shared_ptr<CellTracker::Project> proj)
{
    this->proj = proj;
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
    qDebug() << "requested size" << requestedSize;
    /* Get the image path and the current slider value. */
    if(mouseArea) {
        path = mouseArea->property("path").toString();
        imageNumber = mouseArea->property("sliderValue").toInt() - 1;
    }

    DataProvider dataProvider;
    newImage = dataProvider.requestImage(path, imageNumber);
    QPainter painter(&newImage);

    QPen pen(Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

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
        else if(mouseArea->property("strategy") == "cell division")
            strategy = 2;
        else if(mouseArea->property("strategy") == "change track status")
            strategy = 3;
    }

    bool cellHasBeenSelected = false;
    bool cellHasBeenHovered = false;

    /* Loads the cell objects. */
    listOfPolygons.clear();
    std::shared_ptr<CellTracker::Frame> frame = proj->getMovie()->getFrame(imageNumber);
    for(std::shared_ptr<CellTracker::Slice> slice : frame->getSlices()) {
        for(std::shared_ptr<CellTracker::Object> object : slice->getObjects()) {
            listOfPolygons << object;
        }
    }

    /* Iterate through all cells of the current frame. */
    for(int i = 0; i < listOfPolygons.size(); ++i) {
        QPolygon polygon;
        std::shared_ptr<CellTracker::Object> object = listOfPolygons.at(i);
        pen.setBrush(Qt::black);

        /* The cell is painted green in a tracklet and yellow in an auto tracklet. */
        if(object->isInTracklet()) isInTracklet = true;
        else isInTracklet = false;

        if(selectedCell) {
            if(object->isInTracklet()) {
                trackID = object->getTrackId();
                selectedTrackID = selectedCell->getTrackId();
            }
            else {
                if(selectedCell->isInTracklet()) {
                    if(imageNumber >= selectedCell->getFrameId())
                        trackID = object->getAutoId();
                    else
                        trackID = -1;
                    selectedTrackID = selectedCell->getAutoId();
                }
                else {
                    trackID = object->getAutoId();
                    selectedTrackID = selectedCell->getAutoId();
                }
            }
        }
        else {
            trackID = -2;
            selectedTrackID = -1;
        }

        /* Get the outlines of the cell. */
        std::shared_ptr<QRect> rect = object->getBoundingBox();
        for(QPointF point: object->getOutline()->toStdVector()) {
            //polygon << QPoint(point.x(), point.y());
            int x = rect->topLeft().x() - rect->topLeft().y() + point.y();
            int y = rect->topLeft().y() - rect->topLeft().x() + point.x();
            polygon << QPoint(x, y);
        }

        /* If a cell has been selected, save its ID and frame number. */
        int color = 0;
        if(polygon.containsPoint(mousePosition, Qt::OddEvenFill) && action == 1) {
            cellHasBeenSelected = true;
            if(isInTracklet) color = 1;
            else color = 2;
            pen.setBrush(Qt::red);
            selectedCell = object;
            selectedCellID = object->getId();
            currentImage = imageNumber;

            /* If tracklets shall be combined, remember the first cell and
               jump to the last frame of its tracklet. Then wait until the
               second cell has been selected. */
            if(strategy == 1) {
                if(strategyStep == 1) {
                    strategyStep = 2;
                    lastObject = object;
                    mouseArea->setProperty("jumpStrategy", "combine");
                    mouseArea->setProperty("status", "Select following cell object");
                }
                else {
                    uint32_t objectId = lastObject->getId();
                    std::shared_ptr<CellTracker::Object> lastTrackObject;
                    lastTrackObject = proj->getMovie()->getFrame(imageNumber - 1)->getSlice(0)->getObject(objectId);
                    proj->getGenealogy()->connectObjects(lastObject, lastTrackObject);
                    proj->getGenealogy()->connectObjects(lastTrackObject, selectedCell);
                    mouseArea->setProperty("status", "Tracklets combined - Select cell object");

                    lastObject = selectedCell;
                    mouseArea->setProperty("jumpStrategy", "combine");
                    mouseArea->setProperty("status", "Select following cell object");
                }
            }

            /* If a cell division shall be added, wait until
               the daugter cells have been selected. */
            else if(strategy == 2) {
                if(strategyStep == 1) {
                    strategyStep = 2;
                    motherCell = selectedCell;
                    daughterCells.clear();
                    mouseArea->setProperty("status", "Select daughter objects - press space when finished");
                }
                else if(strategyStep == 2) {
                    if(!daughterCells.contains(selectedCell))
                        daughterCells << selectedCell;
                    else
                        daughterCells.removeAll(selectedCell);
                }
            }
            else if(strategy == 3) {
                mouseArea->setProperty("status", "Select status");
            }
            else
                status = "";
        }

        /* If you hovered over a cell, get its object and track ID. */
        if(polygon.containsPoint(mousePosition, Qt::OddEvenFill)) {
                //&& action == 2) {
            cellHasBeenHovered = true;
            if(isInTracklet) color = 1;
            else color = 2;
            objectID = object->getId();
            currentCell = object;
            if(trackID == selectedTrackID && currentImage == imageNumber)
                pen.setBrush(Qt::red);
        }

        else if(daughterCells.contains(object)) {
            color = 5;
        }

        /* The selected cell is painted with red border in the current frame. */
        else if(action != 1 && trackID == selectedTrackID && currentImage == imageNumber) {
            if(isInTracklet) color = 1;
            else color = 2;
            pen.setBrush(Qt::red);
        }
        /* The selected cell is painted with black border in other frames. */
        else if(action != 1 && trackID == selectedTrackID) {
            if(isInTracklet) color = 1;
            else color = 2;
        }

        /* Cells in tracklets are always painted. */
        else if(isInTracklet) {
            color = 4;
        }
        else
            color = 0;

        /* Draw the outlines of the cell. */
        painter.setPen(pen);
        painter.drawPolygon(polygon);

        QBrush brush;
        QPainterPath path;
        brush.setStyle(Qt::SolidPattern);

        /* Paint the current cell. */
        if(color == 0)
            brush.setColor(Qt::gray);
        else if(color == 1)
            brush.setColor(Qt::green);
        else if(color == 2)
            brush.setColor(Qt::yellow);
        else if(color == 3)
            brush.setColor(Qt::red);
        else if(color == 4)
            brush.setColor(Qt::cyan);
        else if(color == 5)
            brush.setColor(Qt::blue);

        path.addPolygon(polygon);
        painter.fillPath(path, brush);
    }

    /* If no cell has been hovered or selected, delete the current ID. */
    if(selectedCellID == -1 && !cellHasBeenHovered && !cellHasBeenSelected) {
        objectID = -1;
        currentCell = NULL;
    }
    /* In case of clicking between cells, delete the selected cell. */
    else if(action == 1 && !cellHasBeenSelected) {
        objectID = -1;
        selectedCellID = -1;
        currentCell = NULL;
        selectedCell = NULL;
    }
    /* In case of hovering between cells, do not show the ID of the selected cell. */
    else if(!cellHasBeenHovered) {
        objectID = -1; //selectedCellID;
        currentCell = NULL;
    }

    qDebug() << "image size" << newImage.size();
    return newImage;
}

/*!
 * \brief Returns the current cell.
 * \return current cell object
 */
std::shared_ptr<CellTracker::Object> ImageProvider::getCurrentCell()
{
    return currentCell;
}

/*!
 * \brief Returns the selected cell.
 * \return selected cell object
 */
std::shared_ptr<CellTracker::Object> ImageProvider::getSelectedCell()
{
    return selectedCell;
}
