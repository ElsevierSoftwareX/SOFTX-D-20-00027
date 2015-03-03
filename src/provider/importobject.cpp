#include <QtDebug>

#include "importobject.h"

int ImportObject::getMaximumValue()
{
    return maximumValue;
}

int ImportObject::getObjectID()
{
    return imageProvider->getObjectID();
}

int ImportObject::getTrackID()
{
    return imageProvider->getTrackID();
}

int ImportObject::getTrackStart(int id)
{
    QList<int> listOfFrames = getTrackletFrames(id);
    return listOfFrames.first() + 1;
}

int ImportObject::getTrackEnd(int id)
{
    QList<int> listOfFrames = getTrackletFrames(id);
    return listOfFrames.last() + 1;
}

int ImportObject::getTrackLength(int id)
{
    QList<int> listOfFrames = getTrackletFrames(id);
    return listOfFrames.last() - listOfFrames.first() + 1;
}

void ImportObject::readData()
{
    imageProvider->listOfImages.clear();
    imageProvider->listOfImageColors.clear();

    for(int i = 0; i < proj->getMovie()->getFrames().size(); ++i) {
        std::shared_ptr<CellTracker::Frame> frame = proj->getMovie()->getFrame(i);

        for(std::shared_ptr<CellTracker::Slice> slice : frame->getSlices()) {

            for(std::shared_ptr<CellTracker::Object> object : slice->getObjects()) {
                imageProvider->listOfPolygons << object;
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

QList<int> ImportObject::getTrackletFrames(int id)
{
    QList<int> listOfFrames;
    std::shared_ptr<CellTracker::Tracklet> tracklet = proj->getGenealogy()->getTracklet(id);
    for (QPair<std::shared_ptr<CellTracker::Frame>, std::shared_ptr<CellTracker::Object>> p : tracklet->getContained()) {
        listOfFrames << p.first->getID();
    }
    qSort(listOfFrames);
    return listOfFrames;
}

QImage ImportObject::requestImage(QString fileName, int imageNumber)
{
    QUrl url(fileName);
    std::shared_ptr<QImage> img;
    img = MyImport.requestImage(url.toLocalFile(), imageNumber, 0, 0);
    return *img.get();
}
