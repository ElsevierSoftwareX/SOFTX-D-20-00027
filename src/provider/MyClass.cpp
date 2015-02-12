#include <QtDebug>
#include <QPainter>

#include "imageprovider.h"

void MyClass::clickedButton(QString fileUrl)
{
    qDebug() << fileUrl;
}

ImageProvider::ImageProvider() :
    QQuickImageProvider(Image)
{
    oldImage = new QImage();
    mouseArea = NULL;
    imageNumber = 0;
    readData();
}

ImageProvider::~ImageProvider()
{
    if(oldImage != NULL) {
        delete oldImage;
        oldImage = NULL;
    }
}

void ImageProvider::setMouseArea(QObject *area)
{
    mouseArea = area;
}

void ImageProvider::readData()
{
    listOfPoints << QPoint(0, 85) << QPoint(75, 75) << QPoint(100, 10);
    listOfPolygons << listOfPoints;
    listOfPoints.clear();
    listOfColors << 0;

    listOfPoints << QPoint(100, 185) << QPoint(175, 175) << QPoint(110, 200);
    listOfPolygons << listOfPoints;
    listOfPoints.clear();
    listOfColors << 0;

    listOfImages << listOfPolygons;
    listOfPolygons.clear();
    listOfImageColors << listOfColors;
    listOfColors.clear();
}

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    localFile = QUrl(id).toLocalFile();
    oldImage->load(localFile, 0);

    newImage = oldImage->convertToFormat(QImage::Format_RGB32);
    QPainter painter(&newImage);

    QPen pen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);

    if(mouseArea)
        imageNumber = 0;//mouseArea->property("sliderValue").toInt() - 1;

    for(int i = 0; i < listOfImages.at(imageNumber).size(); ++i) {
        QPolygon polygon;
        QList<QPoint> points = listOfImages.at(imageNumber).at(i);

        for(int j = 0; j < points.size(); j++)
            polygon << points[j];
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

        int newColor = 1;
        int oldColor = listOfImageColors.at(imageNumber).at(i);
        if(polygon.containsPoint(mousePosition, Qt::OddEvenFill) && action == 1)
            newColor = 1;
        else if(polygon.containsPoint(mousePosition, Qt::OddEvenFill) && oldColor != 1 && action == 2)
            newColor = 2;
        else if(oldColor != 1)
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

        path.addPolygon(polygon);
        painter.fillPath(path, brush);
    }

    return newImage;
}
