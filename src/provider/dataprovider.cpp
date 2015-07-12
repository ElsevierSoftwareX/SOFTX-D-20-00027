#include <QtConcurrent/QtConcurrent>
#include <QtDebug>

#include "dataprovider.h"
#include "messagerelay.h"
#include "guistate.h"

namespace CellTracker {

DataProvider *DataProvider::theInstance = nullptr;

DataProvider *DataProvider::getInstance(){
    if (!theInstance)
        theInstance = new DataProvider();
    return theInstance;
}

DataProvider::DataProvider(QObject *parent) : QObject(parent) {}
double DataProvider::getScaleFactor() const
{
    return scaleFactor;
}

void DataProvider::setScaleFactor(double value)
{
    scaleFactor = value;
}

//QString DataProvider::getJumpStrategy() const
//{
//    return jumpStrategy;
//}

//void DataProvider::setJumpStrategy(const QString &value)
//{
//    jumpStrategy = value;
//}

//QString DataProvider::getStrategy() const
//{
//    return strategy;
//}

//void DataProvider::setStrategy(const QString &value)
//{
//    strategy = value;
//}


QObject *DataProvider::qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return getInstance();
}

///*!
// * \brief Returns the number of frames in the movie.
// * \return maximum slider value
// */
//int DataProvider::getMaximumValue()
//{
//    return maximumValue;
//}

///*!
// * \brief Returns the object ID of the current cell.
// * \return object ID
// */
//int DataProvider::getCurrentObjectID()
//{
//    return GUIState::getInstance()->getObjectID();
//}

///*!
// * \brief Returns the object ID of the selected cell.
// * \return object ID
// */
//int DataProvider::getSelectedObjectID()
//{
//    return GUIState::getInstance()->getNewSelectedCellID();
//}

///*!
// * \brief Returns the track ID of the current cell.
// * \return track ID
// */
//int DataProvider::getCurrentTrackID()
//{
//    return GUIState::getInstance()->getCurrentCell()->getTrackId();
//}

///*!
// * \brief Returns the track ID of the selected cell.
// * \return track ID
// */
//int DataProvider::getSelectedTrackID()
//{
//    return GUIState::getInstance()->getNewSelectedCell()->getTrackId();
//}

///*!
// * \brief Returns the start frame of a track.
// * \param id is the track id
// * \return track start
// */
//int DataProvider::getTrackStart(int id)
//{
//    QList<int> listOfFrames = getTrackletFrames(id);
//    return listOfFrames.first() + 1;
//}

///*!
// * \brief Returns the end frame of a track.
// * \param id is the track id
// * \return track end
// */
//int DataProvider::getTrackEnd(int id)
//{
//    QList<int> listOfFrames = getTrackletFrames(id);
//    return listOfFrames.last() + 1;
//}

///*!
// * \brief Returns the length of a track.
// * \param id is the track id
// * \return track length
// */
//int DataProvider::getTrackLength(int id)
//{
//    QList<int> listOfFrames = getTrackletFrames(id);
//    return listOfFrames.last() - listOfFrames.first() + 1;
//}

///*!
// * \brief Returns the autotrack ID of the current cell.
// * \return track ID
// */
//int DataProvider::getCurrentAutoTrackID()
//{
//    return GUIState::getInstance()->getCurrentCell()->getAutoId();
//}

///*!
// * \brief Returns the autotrack ID of the selected cell.
// * \return track ID
// */
//int DataProvider::getSelectedAutoTrackID()
//{
//    std::shared_ptr<Object> selected = GUIState::getInstance()->getNewSelectedCell();
//    return selected?selected->getAutoId():UINT32_MAX;
//}

///*!
// * \brief Returns the start frame of an autotrack.
// * \param id is the track id
// * \return track start
// */
//int DataProvider::getAutoTrackStart(int id)
//{
//    QList<int> listOfFrames = getAutoTrackletFrames(id);
//    if (!listOfFrames.empty())
//        return listOfFrames.first() + 1;
//    else
//        return 0;
//}

///*!
// * \brief Returns the end frame of an autotrack.
// * \param id is the track id
// * \return track end
// */
//int DataProvider::getAutoTrackEnd(int id)
//{
//    QList<int> listOfFrames = getAutoTrackletFrames(id);
//    if (!listOfFrames.empty())
//        return listOfFrames.last() + 1;
//    else
//        return 0;
//}

///*!
// * \brief Returns the length of an autotrack.
// * \param id is the track id
// * \return track length
// */
//int DataProvider::getAutoTrackLength(int id)
//{
//    QList<int> listOfFrames = getAutoTrackletFrames(id);
//    if (!listOfFrames.empty())
//        return listOfFrames.last() - listOfFrames.first() + 1;
//    else
//        return 0;
//}

//bool DataProvider::connectTracks()
//{
//    if(GUIState::getInstance()->getNewSelectedCell() && GUIState::getInstance()->getCurrentCell()) {
//        std::shared_ptr<Object> firstObject = GUIState::getInstance()->getNewSelectedCell();
//        std::shared_ptr<Object> secondObject = GUIState::getInstance()->getCurrentCell();
//        if(proj->getGenealogy()->connectObjects(firstObject, secondObject))
//            return true;
//        else
//            return false;
//    }
//    else
//        return false;
//}

///*!
// * \brief Decides whether the current object belongs to a tracklet.
// * \return true or false
// */
//bool DataProvider::isCurrentInTracklet()
//{
//    return GUIState::getInstance()->getCurrentCell()->isInTracklet();
//}

///*!
// * \brief Decides whether the selected object belongs to a tracklet.
// * \return true or false
// */
//bool DataProvider::isSelectedInTracklet()
//{
//    std::shared_ptr<Object> selected = GUIState::getInstance()->getNewSelectedCell();
//    return selected?selected->isInTracklet():false;
//}

//void DataProvider::setStrategyStep(int step)
//{
//    GUIState::getInstance()->setStrategyStep(step);
//}

//std::shared_ptr<Project> DataProvider::getProj()
//{
//    return proj;
//}

//void DataProvider::setProj(std::shared_ptr<Project> &value) {
//    proj = value;
//}


///*!
// * \brief Changes the track status if a track has been selected.
// * \param status is the new status of the track
// */
//void DataProvider::setStatus(QString status)
//{
//    std::shared_ptr<Object> selectedCell = GUIState::getInstance()->getNewSelectedCell();
//    if(status != "" && selectedCell != nullptr) {
//        std::shared_ptr<Tracklet> track = proj->getGenealogy()->getTracklet(selectedCell->getTrackId());
//        if(status == "open") proj->getGenealogy()->setOpen(track);
//        else if(status == "cell division") proj->getGenealogy()->setOpen(track);
//        else if(status == "dead") proj->getGenealogy()->setOpen(track);
//        else if(status == "lost") proj->getGenealogy()->setOpen(track);
//        else if(status == "end of movie reached") proj->getGenealogy()->setOpen(track);
//    }
//}

/*!
 * \brief Loads a HDF5 file and reads the necessary data.
 * \param fileName is the name of the HDF5 file
 */
void DataProvider::runLoadHDF5(QString fileName) {
    QUrl url(fileName);
    std::shared_ptr<Project> proj = importer.load(url.toLocalFile());
    GUIState::getInstance()->setNewProj(proj);
    GUIState::getInstance()->setNewMaximumFrame(proj->getMovie()->getFrames().size()-1);
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
    std::shared_ptr<Project> proj = GUIState::getInstance()->getNewProj();
    exporter.save(proj, url.toLocalFile());
    GUIState::getInstance()->setNewMaximumFrame(proj->getMovie()->getFrames().size()-1);
}

///*!
// * \brief Returns the current entry of the status bar.
// * \return status entry
// */
//QString DataProvider::getStatus()
//{
//    return GUIState::getInstance()->getStatus();
//}

///*!
// * \brief Returns a list of frames of a track.
// * \param id is the track id
// * \return a QList<int> that contains the track frames
// */
//QList<int> DataProvider::getTrackletFrames(int id)
//{
//    QList<int> listOfFrames;
//    std::shared_ptr<Tracklet> a = proj->getGenealogy()->getTracklet(id);
//    for(QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>> p : a->getContained()) {
//        listOfFrames << p.first->getID();
//    }
//    qSort(listOfFrames);
//    return listOfFrames;
//}

///*!
// * \brief Returns a list of frames of an autotrack.
// * \param id is the track id
// * \return a QList<int> that contains the track frames
// */
//QList<int> DataProvider::getAutoTrackletFrames(int id)
//{
//    QList<int> listOfFrames;
//    std::shared_ptr<AutoTracklet> a = proj->getAutoTracklet(id);
//    if (a)
//        for(QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>> p : a->getComponents())
//        listOfFrames << p.first->getID();

//    qSort(listOfFrames);
//    return listOfFrames;
//}

std::shared_ptr<Object> DataProvider::cellAtFrame(int frame, double x, double y) {
    std::shared_ptr<Project> proj = GUIState::getInstance()->getNewProj();
    if (!proj)
        return nullptr;

    QList<std::shared_ptr<Slice>> slices = proj->getMovie()
            ->getFrame(frame)
            ->getSlices();

    QPointF p = QPoint(x,y) / DataProvider::getInstance()->getScaleFactor();

    for (std::shared_ptr<Slice> s : slices)
        for (std::shared_ptr<Object> o : s->getObjects().values())
            if (o->getOutline()->containsPoint(p, Qt::OddEvenFill))
                return o;

    return nullptr;
}

std::shared_ptr<Object> DataProvider::cellAt(double x, double y) {
    int currentFrame = GUIState::getInstance()->getNewCurrentFrame();
    return cellAtFrame(currentFrame, x, y);
}

int DataProvider::cellIDAt(double x, double y) {
    std::shared_ptr<Object> o = cellAt(x,y);
    return o?static_cast<int>(o->getId()):INT_MAX;
}


/*!
 * \brief Returns a list of annotaions.
 * \return a QList<QPair<QString, QString>> that contains the annotations
 */
QList<QPair<QString, QString>> DataProvider::getAnnotations()
{
    QList<QPair<QString, QString>> listOfAnnotations;
    std::shared_ptr<QList<std::shared_ptr<Annotation>>> annotations = GUIState::getInstance()->getNewProj()->getGenealogy()->getAnnotations();
    for(std::shared_ptr<Annotation> annotation : *annotations) {
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

}
