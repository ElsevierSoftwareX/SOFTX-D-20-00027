#include <QtDebug>

#include "importobject.h"

/*!
 * \brief Returns the number of frames in the movie.
 * \return maximum slider value
 */
int ImportObject::getMaximumValue()
{
    return maximumValue;
}

/*!
 * \brief Returns the object ID of the current cell.
 * \return object ID
 */
int ImportObject::getObjectID()
{
    return imageProvider->getObjectID();
}

/*!
 * \brief Returns the track ID of the current cell.
 * \return track ID
 */
int ImportObject::getTrackID()
{
    return imageProvider->getTrackID();
}

/*!
 * \brief Returns the start frame of a track.
 * \param id is the track id
 * \return track start
 */
int ImportObject::getTrackStart(int id)
{
    QList<int> listOfFrames = getTrackletFrames(id);
    return listOfFrames.first() + 1;
}

/*!
 * \brief Returns the end frame of a track.
 * \param id is the track id
 * \return track end
 */
int ImportObject::getTrackEnd(int id)
{
    QList<int> listOfFrames = getTrackletFrames(id);
    return listOfFrames.last() + 1;
}

/*!
 * \brief Returns the length of a track.
 * \param id is the track id
 * \return track length
 */
int ImportObject::getTrackLength(int id)
{
    QList<int> listOfFrames = getTrackletFrames(id);
    return listOfFrames.last() - listOfFrames.first() + 1;
}

/*!
 * \brief Loads the cell objects and initializes their corresponding colors.
 */
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

/*!
 * \brief Loads an HDF5 file and reads the necessary data.
 * \param fileName is the name of the HDF5 file
 */
void ImportObject::loadHDF5(QString fileName)
{
    QUrl url(fileName);
    proj = MyImport.load(url.toLocalFile());
    maximumValue = proj->getMovie()->getFrames().size();
    readData();
}

/*!
 * \brief Returns a list of frames of a track.
 * \param id is the track id
 * \return a QList<int> that contains the track frames
 */
QList<int> ImportObject::getTrackletFrames(int id)
{
    QList<int> listOfFrames;
    std::shared_ptr<CellTracker::Tracklet> tracklet = proj->getGenealogy()->getTracklet(id);
    for(QPair<std::shared_ptr<CellTracker::Frame>, std::shared_ptr<CellTracker::Object>> p : tracklet->getContained()) {
        listOfFrames << p.first->getID();
    }
    qSort(listOfFrames);
    return listOfFrames;
}

/*!
 * \brief Returns a list of annotaions.
 * \return a QList<QPair<QString, QString>> that contains the annotations
 */
QList<QPair<QString, QString>> ImportObject::getAnnotations()
{
    QList<QPair<QString, QString>> listOfAnnotations;
    std::shared_ptr<QList<CellTracker::Annotation>> annotations = proj->getGenealogy()->getAnnotations();
    for(CellTracker::Annotation annotation : *annotations) {
        QString name = QString::fromStdString(annotation.getText());
        QString description = QString::fromStdString(annotation.getText());
        listOfAnnotations << QPair<QString, QString>(name, description);
    }
    return listOfAnnotations;
}

/*!
 * \brief Returns an image of an HDF5 file.
 * \param fileName is the name of the HDF5 file
 * \param imageNumber is the number of the frame
 * \return the requested QImage
 */
QImage ImportObject::requestImage(QString fileName, int imageNumber)
{
    QUrl url(fileName);
    std::shared_ptr<QImage> img;
    img = MyImport.requestImage(url.toLocalFile(), imageNumber, 0, 0);
    return *img.get();
}
