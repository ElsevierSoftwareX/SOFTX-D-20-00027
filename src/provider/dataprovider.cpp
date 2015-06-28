#include <QtConcurrent/QtConcurrent>
#include <QtDebug>

#include "dataprovider.h"
#include "messagerelay.h"
#include "guistate.h"

/*!
 * \brief Returns the number of frames in the movie.
 * \return maximum slider value
 */
int DataProvider::getMaximumValue()
{
    return maximumValue;
}

/*!
 * \brief Returns the object ID of the current cell.
 * \return object ID
 */
int DataProvider::getCurrentObjectID()
{
    return CellTracker::GUIState::getInstance()->getObjectID();
}

/*!
 * \brief Returns the object ID of the selected cell.
 * \return object ID
 */
int DataProvider::getSelectedObjectID()
{
    return CellTracker::GUIState::getInstance()->getSelectedCellID();
}

/*!
 * \brief Returns the track ID of the current cell.
 * \return track ID
 */
int DataProvider::getCurrentTrackID()
{
    return CellTracker::GUIState::getInstance()->getCurrentCell()->getTrackId();
}

/*!
 * \brief Returns the track ID of the selected cell.
 * \return track ID
 */
int DataProvider::getSelectedTrackID()
{
    return CellTracker::GUIState::getInstance()->getSelectedCell()->getTrackId();
}

/*!
 * \brief Returns the start frame of a track.
 * \param id is the track id
 * \return track start
 */
int DataProvider::getTrackStart(int id)
{
    QList<int> listOfFrames = getTrackletFrames(id);
    return listOfFrames.first() + 1;
}

/*!
 * \brief Returns the end frame of a track.
 * \param id is the track id
 * \return track end
 */
int DataProvider::getTrackEnd(int id)
{
    QList<int> listOfFrames = getTrackletFrames(id);
    return listOfFrames.last() + 1;
}

/*!
 * \brief Returns the length of a track.
 * \param id is the track id
 * \return track length
 */
int DataProvider::getTrackLength(int id)
{
    QList<int> listOfFrames = getTrackletFrames(id);
    return listOfFrames.last() - listOfFrames.first() + 1;
}

/*!
 * \brief Returns the autotrack ID of the current cell.
 * \return track ID
 */
int DataProvider::getCurrentAutoTrackID()
{
    return CellTracker::GUIState::getInstance()->getCurrentCell()->getAutoId();
}

/*!
 * \brief Returns the autotrack ID of the selected cell.
 * \return track ID
 */
int DataProvider::getSelectedAutoTrackID()
{
    return CellTracker::GUIState::getInstance()->getSelectedCell()->getAutoId();
}

/*!
 * \brief Returns the start frame of an autotrack.
 * \param id is the track id
 * \return track start
 */
int DataProvider::getAutoTrackStart(int id)
{
    QList<int> listOfFrames = getAutoTrackletFrames(id);
    return listOfFrames.first() + 1;
}

/*!
 * \brief Returns the end frame of an autotrack.
 * \param id is the track id
 * \return track end
 */
int DataProvider::getAutoTrackEnd(int id)
{
    QList<int> listOfFrames = getAutoTrackletFrames(id);
    return listOfFrames.last() + 1;
}

/*!
 * \brief Returns the length of an autotrack.
 * \param id is the track id
 * \return track length
 */
int DataProvider::getAutoTrackLength(int id)
{
    QList<int> listOfFrames = getAutoTrackletFrames(id);
    return listOfFrames.last() - listOfFrames.first() + 1;
}

bool DataProvider::connectTracks()
{
    if(CellTracker::GUIState::getInstance()->getSelectedCell() && CellTracker::GUIState::getInstance()->getCurrentCell()) {
        std::shared_ptr<CellTracker::Object> firstObject = CellTracker::GUIState::getInstance()->getSelectedCell();
        std::shared_ptr<CellTracker::Object> secondObject = CellTracker::GUIState::getInstance()->getCurrentCell();
        if(proj->getGenealogy()->connectObjects(firstObject, secondObject))
            return true;
        else
            return false;
    }
    else
        return false;
}

/*!
 * \brief Decides whether the current object belongs to a tracklet.
 * \return true or false
 */
bool DataProvider::isCurrentInTracklet()
{
    return CellTracker::GUIState::getInstance()->getCurrentCell()->isInTracklet();
}

/*!
 * \brief Decides whether the selected object belongs to a tracklet.
 * \return true or false
 */
bool DataProvider::isSelectedInTracklet()
{
    return CellTracker::GUIState::getInstance()->getSelectedCell()->isInTracklet();
}

void DataProvider::setMotherCell()
{
    imageProvider->setMotherCell();
}

void DataProvider::setDaughterCells()
{
    imageProvider->setDaughterCells();
}

void DataProvider::setStrategyStep(int step)
{
    CellTracker::GUIState::getInstance()->setStrategyStep(step);
}

void DataProvider::setProvider(ImageProvider2 *provider)
{
    imageProvider = provider;
}

/*!
 * \brief Changes the track status if a track has been selected.
 * \param status is the new status of the track
 */
void DataProvider::setStatus(QString status)
{
    std::shared_ptr<CellTracker::Object> selectedCell = CellTracker::GUIState::getInstance()->getSelectedCell();
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
 * \brief Loads a HDF5 file and reads the necessary data.
 * \param fileName is the name of the HDF5 file
 */
void DataProvider::runLoadHDF5(QString fileName) {
    QUrl url(fileName);
    proj = importer.load(url.toLocalFile());
    maximumValue = proj->getMovie()->getFrames().size();
    CellTracker::GUIState::getInstance()->setProj(proj);
    MessageRelay::emitFinishNotification();
}

/*!
 * \brief Asynchronously calls the method to load a project from HDF5
 * \param fileName is the name of the HDF5 file
 */
void DataProvider::loadHDF5(QString fileName)
{
    QtConcurrent::run(this, &DataProvider::runLoadHDF5, fileName);
}

/*!
 * \brief Writes the project to a HDF5 file
 * \param fileName is the name of the HDF5 file
 */
void DataProvider::saveHDF5(QString fileName)
{
    QUrl url(fileName);
    exporter.save(proj, url.toLocalFile());
    maximumValue = proj->getMovie()->getFrames().size();
    CellTracker::GUIState::getInstance()->setProj(proj);
}

/*!
 * \brief Returns the current entry of the status bar.
 * \return status entry
 */
QString DataProvider::getStatus()
{
    return CellTracker::GUIState::getInstance()->getStatus();
}

/*!
 * \brief Returns a list of frames of a track.
 * \param id is the track id
 * \return a QList<int> that contains the track frames
 */
QList<int> DataProvider::getTrackletFrames(int id)
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
QList<int> DataProvider::getAutoTrackletFrames(int id)
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
QList<QPair<QString, QString>> DataProvider::getAnnotations()
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
QImage DataProvider::requestImage(QString fileName, int imageNumber)
{
    QUrl url(fileName);
    std::shared_ptr<QImage> img;
    img = importer.requestImage(url.toLocalFile(), imageNumber, 0, 0);
    return *img.get();
}
