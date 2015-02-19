#include <QtDebug>

#include "importobject.h"

int ImportObject::getMaximumValue()
{
    return maximumValue;
}

void ImportObject::readData()
{
    imageProvider->listOfImages.clear();
    imageProvider->listOfImageColors.clear();

    for(int i = 0; i < proj->getMovie()->getFrames().size(); ++i) {
        std::shared_ptr<Frame> frame = proj->getMovie()->getFrame(i);

        for(std::shared_ptr<Slice> slice : frame->getSlices()) {

            for(std::shared_ptr<Object> object : slice->getObjects()) {

                for(QPointF point: object->getOutline()->toStdVector()) {
                    imageProvider->listOfPoints << QPoint(point.x(), point.y());
                }

                imageProvider->listOfPolygons << imageProvider->listOfPoints;
                imageProvider->listOfPoints.clear();
                imageProvider->listOfColors << 0;
            }
        }

        imageProvider->listOfImages << imageProvider->listOfPolygons;
        imageProvider->listOfPolygons.clear();
        imageProvider->listOfImageColors << imageProvider->listOfColors;
        imageProvider->listOfColors.clear();
    }
}

void ImportObject::setProvider(ImageProvider *provider)
{
    imageProvider = provider;
}

void ImportObject::loadHDF5(QString fileName)
{
    QUrl url(fileName);
    proj = MyImport.load(url.toLocalFile());
    maximumValue = proj->getMovie()->getFrames().size();
    readData();
}

QImage ImportObject::requestImage(QString fileName, int imageNumber)
{
    QUrl url(fileName);
    std::shared_ptr<QImage> img;
    img = MyImport.requestImage(url.toLocalFile(), imageNumber, 0, 0);
    return *img.get();
}
