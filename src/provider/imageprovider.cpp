#include <QtDebug>
#include <QPainter>

#include "imageprovider.h"
#include "src/provider/importobject.h"

ImageProvider::ImageProvider() :
    QQuickImageProvider(Image)
{
    selectedCell = -1;
    currentImage = -1;
    mouseArea = NULL;
    imageNumber = 0;
    objectID = -1;
    trackID = -1;
}

ImageProvider::~ImageProvider()
{
}

int ImageProvider::getObjectID()
{
    return objectID;
}

int ImageProvider::getTrackID()
{
    return trackID;
}

void ImageProvider::setMouseArea(QObject *area)
{
    mouseArea = area;
}

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    if(mouseArea) {
        path = mouseArea->property("path").toString();
        imageNumber = mouseArea->property("sliderValue").toInt() - 1;
    }

    ImportObject MyImport;
    newImage = MyImport.requestImage(path, imageNumber);
    QPainter painter(&newImage);

    QPen pen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);

    if(imageNumber != currentImage)
        currentImage = -1;

    for(int i = 0; i < listOfImages.at(imageNumber).size(); ++i) {
        QPolygon polygon;
        std::shared_ptr<CellTracker::Object> object = listOfImages.at(imageNumber).at(i);

        std::shared_ptr<QRect> rect = object->getBoundingBox();
        for(QPointF point: object->getOutline()->toStdVector()) {
            //polygon << QPoint(point.x(), point.y());
            int x = rect->topLeft().x() - rect->topLeft().y() + point.y();
            int y = rect->topLeft().y() - rect->topLeft().x() + point.x();
            polygon << QPoint(x, y);
        }
        painter.drawPolygon(polygon);

        int action;
        QPoint mousePosition;

        if(mouseArea) {
            if(mouseArea->property("mouseAction") == "leftClick")
                action = 1;
            else if(mouseArea->property("mouseAction") == "hover")
                action = 2;
            mousePosition.setX(mouseArea->property("lastX").toInt());
            mousePosition.setY(mouseArea->property("lastY").toInt());
        }

        int newColor = 0;
        int oldColor = listOfImageColors.at(imageNumber).at(i);
        if(polygon.containsPoint(mousePosition, Qt::OddEvenFill) && action == 1) {
            newColor = 3;
            selectedCell = object->getID();
            currentImage = imageNumber;
        }
        else if(polygon.containsPoint(mousePosition, Qt::OddEvenFill) && action == 2) {
            newColor = 3;
            objectID = object->getID();
            trackID = 1;//object->getTrackID(); // TrackID nicht korrekt eingelesen
        }
        else if(object->getID() == selectedCell && currentImage == imageNumber)
            newColor = 3;
        else if(object->getID() == selectedCell)
            newColor = 2;
        else
            newColor = 0;
        listOfImageColors[imageNumber].replace(i, newColor);

        QBrush brush;
        QPainterPath path;
        brush.setStyle(Qt::SolidPattern);

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

    return newImage;
}
