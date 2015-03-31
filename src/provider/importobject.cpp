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
    return getTrackID(imageProvider->getObjectID());
}

/*!
 * \brief Returns the track ID of the current cell.
 * \param id is the object id
 * \return track ID
 */
int ImportObject::getTrackID(int id)
{
    for(std::shared_ptr<CellTracker::AutoTracklet> a : proj->getAutoTracklets()) {
        for(QPair<std::shared_ptr<CellTracker::Frame>, std::shared_ptr<CellTracker::Object>> p : a->getComponents()) {
            if(id == (int)p.second->getID())
                return a->getID();
            else
                break;
        }
    }
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

bool ImportObject::connectTracks()
{
    eventTriggered();
    if(imageProvider->getSelectedCellID() != -1 && imageProvider->getObjectID() != -1) {
        qDebug() << "cell1: id" << imageProvider->getSelectedCellID();
        qDebug() << "cell2: id" << imageProvider->getObjectID();
        qDebug() << "cell1: frame id" << imageProvider->getCurrentImage();
        qDebug() << "cell2: frame id" << imageProvider->getImageNumber();
        qDebug() << "cell1: track id" << this->getTrackID(imageProvider->getSelectedCellID());
        qDebug() << "cell2: track id" << this->getTrackID(imageProvider->getObjectID());
        qDebug() << "cell2: id" << imageProvider->getCurrentCell()->getID();
        //int frame = imageProvider->getImageNumber();
        //int track = getTrackID(imageProvider->getSelectedCellID());
        //std::shared_ptr<CellTracker::Object> object = imageProvider->getCurrentCell();
        //proj->getGenealogy()->addObject(frame, track, object);
        return true;
    }
    else
        return false;
}

void ImportObject::setLastObjectID(int id)
{
    imageProvider->setLastObjectID(id);
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
    imageProvider->setProject(proj);
}

/*!
 * \brief Returns the current entry of the status bar.
 * \return status entry
 */
QString ImportObject::getStatus()
{
    return imageProvider->getStatus();
}

/*!
 * \brief Returns a list of frames of a track.
 * \param id is the track id
 * \return a QList<int> that contains the track frames
 */
QList<int> ImportObject::getTrackletFrames(int id)
{
    QList<int> listOfFrames;
    std::shared_ptr<CellTracker::AutoTracklet> a = proj->getAutoTracklet(id);
    for(QPair<std::shared_ptr<CellTracker::Frame>, std::shared_ptr<CellTracker::Object>> p : a->getComponents()) {
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
    std::shared_ptr<QList<std::shared_ptr<CellTracker::Annotation>>> annotations = proj->getGenealogy()->getAnnotations();
    for(std::shared_ptr<CellTracker::Annotation> annotation : *annotations) {
        QString name = QString::fromStdString(annotation->getAnnotationText());
        QString description = QString::fromStdString(annotation->getAnnotationText());
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
