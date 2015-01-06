#include <QtDebug>

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

    //Thresholding currentThresholding = Thresholding(oldImage);
    //QImage newImage = currentThresholding.changeThreshold(threshold);
    //newImage.scaled(requestedSize, Qt::KeepAspectRatio);

    //size->setHeight(newImage.height());
    //size->setWidth(newImage.width());

    //delete oldImage;
    //oldImage = NULL;

    lastImage = oldImage;

    //lastImage = &newImage;
    return *lastImage;
}
