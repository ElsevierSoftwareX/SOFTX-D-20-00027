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
int ImportObject::getCurrentObjectID()
{
    return imageProvider->getObjectID();
}

/*!
 * \brief Returns the object ID of the selected cell.
 * \return object ID
 */
int ImportObject::getSelectedObjectID()
{
    return imageProvider->getSelectedCellID();
}

/*!
 * \brief Returns the track ID of the current cell.
 * \return track ID
 */
int ImportObject::getCurrentTrackID()
{
    return imageProvider->getCurrentCell()->getTrackId();
}

/*!
 * \brief Returns the track ID of the selected cell.
 * \return track ID
 */
int ImportObject::getSelectedTrackID()
{
    return imageProvider->getSelectedCell()->getTrackId();
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
 * \brief Returns the autotrack ID of the current cell.
 * \return track ID
 */
int ImportObject::getCurrentAutoTrackID()
{
    return imageProvider->getCurrentCell()->getAutoId();
}

/*!
 * \brief Returns the autotrack ID of the selected cell.
 * \return track ID
 */
int ImportObject::getSelectedAutoTrackID()
{
    return imageProvider->getSelectedCell()->getAutoId();
}

/*!
 * \brief Returns the start frame of an autotrack.
 * \param id is the track id
 * \return track start
 */
int ImportObject::getAutoTrackStart(int id)
{
    QList<int> listOfFrames = getAutoTrackletFrames(id);
    return listOfFrames.first() + 1;
}

/*!
 * \brief Returns the end frame of an autotrack.
 * \param id is the track id
 * \return track end
 */
int ImportObject::getAutoTrackEnd(int id)
{
    QList<int> listOfFrames = getAutoTrackletFrames(id);
    return listOfFrames.last() + 1;
}

/*!
 * \brief Returns the length of an autotrack.
 * \param id is the track id
 * \return track length
 */
int ImportObject::getAutoTrackLength(int id)
{
    QList<int> listOfFrames = getAutoTrackletFrames(id);
    return listOfFrames.last() - listOfFrames.first() + 1;
}

bool ImportObject::connectTracks()
{
    if(imageProvider->getSelectedCell() && imageProvider->getCurrentCell()) {
        //qDebug() << "cell1: id" << imageProvider->getSelectedCell()->getId();
        //qDebug() << "cell2: id" << imageProvider->getCurrentCell()->getId();
        std::shared_ptr<CellTracker::Object> firstObject = imageProvider->getSelectedCell();
        std::shared_ptr<CellTracker::Object> secondObject = imageProvider->getCurrentCell();
        proj->getGenealogy()->connectObjects(firstObject, secondObject);
        return true;
    }
    else
        return false;
}

/*!
 * \brief Decides whether the current object belongs to a tracklet.
 * \return true or false
 */
bool ImportObject::isCurrentInTracklet()
{
    return imageProvider->getCurrentCell()->isInTracklet();
}

/*!
 * \brief Decides whether the selected object belongs to a tracklet.
 * \return true or false
 */
bool ImportObject::isSelectedInTracklet()
{
    return imageProvider->getSelectedCell()->isInTracklet();
}

void ImportObject::setStrategyStep(int step)
{
    imageProvider->setStrategyStep(step);
}

void ImportObject::setProvider(ImageProvider *provider)
{
    imageProvider = provider;
}

/*!
 * \brief Changes the track status if a track has been selected.
 * \param status is the new status of the track
 */
void ImportObject::setStatus(QString status)
{
    std::shared_ptr<CellTracker::Object> selectedCell = imageProvider->getSelectedCell();
    if(status != "" && selectedCell != nullptr) {
        std::shared_ptr<CellTracker::Tracklet> track = proj->getGenealogy()->getTracklet(selectedCell->getTrackId());
        if(status == "open") proj->getGenealogy()->setOpen(track);
        else if(status == "cell division") proj->getGenealogy()->setOpen(track);
        else if(status == "dead") proj->getGenealogy()->setOpen(track);
        else if(status == "lost") proj->getGenealogy()->setOpen(track);
        else if(status == "end of movie reached") proj->getGenealogy()->setOpen(track);
    }
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
    std::shared_ptr<CellTracker::Tracklet> a = proj->getGenealogy()->getTracklet(id);
    for(QPair<std::shared_ptr<CellTracker::Frame>, std::shared_ptr<CellTracker::Object>> p : a->getContained()) {
        listOfFrames << p.first->getID();
    }
    qSort(listOfFrames);
    return listOfFrames;
}

/*!
 * \brief Returns a list of frames of an autotrack.
 * \param id is the track id
 * \return a QList<int> that contains the track frames
 */
QList<int> ImportObject::getAutoTrackletFrames(int id)
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
