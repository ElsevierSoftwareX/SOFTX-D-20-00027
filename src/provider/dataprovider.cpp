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

QObject *DataProvider::qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return getInstance();
}

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
            if (o->getBoundingBox()->left() <= p.x()
                    && o->getBoundingBox()->right() >= p.x()
                    && o->getBoundingBox()->top() <= p.y()
                    && o->getBoundingBox()->bottom() >= p.y())
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
