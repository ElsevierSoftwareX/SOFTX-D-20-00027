#include <QtDebug>
#include <QPainter>

#include "imageprovider.h"

ImageProvider::ImageProvider() :
    QQuickImageProvider(Image)
{
    lastImage = NULL;
    horizontalSlider = NULL;
}

ImageProvider::~ImageProvider()
{
    if(lastImage != NULL) {
        delete lastImage;
        lastImage = NULL;
    }
}

void ImageProvider::setSlider(QObject *slider)
{
    horizontalSlider = slider;
}

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QImage *oldImage = new QImage();
    QString localFile;

    if(lastImage != NULL) {
        delete lastImage;
        lastImage = NULL;
    }

    //int threshold = 0;
    //if(horizontalSlider)
    //    threshold = horizontalSlider->property("value").toInt();

    localFile = QUrl(id).toLocalFile();
    oldImage->load(localFile, 0);

    QImage newImage = oldImage->convertToFormat(QImage::Format_RGB32);
    QPainter painter(&newImage);
    QPolygon polygon;
    polygon << QPoint(0, 85) << QPoint(75, 75) << QPoint(100, 10) << QPoint(0, 85);
    // QPen: style(), width(), brush(), capStyle() and joinStyle().
    QPen pen(Qt::red, 3, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);
    // Brush
    //QBrush brush;
    //brush.setColor(Qt::green);
    //brush.setStyle(Qet::SolidPattern);
    // Fill polygon
    //QPainterPath path;
    //path.addPolygon(polygon);
    // Draw polygon
    painter.drawPolygon(polygon);
    //painter.fillPath(path, brush);

    QPoint test(0, 5);
    qDebug() << polygon.containsPoint(test, Qt::OddEvenFill);
    test = QPoint(50, 50);
    qDebug() << polygon.containsPoint(test, Qt::OddEvenFill);

    //Thresholding currentThresholding = Thresholding(oldImage);
    //QImage newImage = currentThresholding.changeThreshold(threshold);
    //newImage.scaled(requestedSize, Qt::KeepAspectRatio);

    //size->setHeight(newImage.height());
    //size->setWidth(newImage.width());

    //delete oldImage;
    //oldImage = NULL;

    //lastImage = oldImage;

    lastImage = &newImage;
    return *lastImage;
}
