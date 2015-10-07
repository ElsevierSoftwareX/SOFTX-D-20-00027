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

void DataProvider::setAnnotations(const QList<QObject *> &value)
{
    if (annotations != value)
        emit annotationsChanged(annotations = value);
}

void DataProvider::addAnnotation()
{
    std::shared_ptr<Annotation> a = std::shared_ptr<Annotation>(new Annotation());
    /*! \todo find another solution */
    GUIState::getInstance()->getProj()->getGenealogy()->addAnnotation(a);
    emit annotationsChanged(annotations);
}

void DataProvider::changeAnnotation(int id, QString title, QString description)
{
    std::shared_ptr<QList<std::shared_ptr<Annotation>>> anno = GUIState::getInstance()->getProj()->getGenealogy()->getAnnotations();
    for (std::shared_ptr<Annotation> a : *anno)
        if (id >= 0 && a->getId() == (uint32_t)id) {
            a->setTitle(title);
            a->setDescription(description);
            emit annotationsChanged(annotations);
        }
}

QList<QObject *> DataProvider::getAnnotations()
{
    QList<QObject*> old = annotations;
    QList<QObject*> ret;

    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    if (!proj)
        return ret;
    std::shared_ptr<Genealogy> gen = proj->getGenealogy();
    if (!gen)
        return ret;
    std::shared_ptr<QList<std::shared_ptr<Annotation>>> ann = gen->getAnnotations();
    if (!ann)
        return ret;
    QList<std::shared_ptr<Annotation>> a = *ann;

    for (std::shared_ptr<Annotation> e : a)
        ret.push_back(e.get());

    /* check if changed */
    if (old.length() == ret.length()) {
        bool changed = false;
        for (int i = 0; i < old.length(); i++)
            if (old[i] != ret[i])
                changed = true;
        if (!changed)
            return old;
    }
    emit annotationsChanged(annotations = ret);
    return ret;
}

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
    GUIState::getInstance()->setProj(proj);
    GUIState::getInstance()->setMaximumFrame(proj->getMovie()->getFrames().size()-1);
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
    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    exporter.save(proj, url.toLocalFile());
    MessageRelay::emitFinishNotification();
    GUIState::getInstance()->setMaximumFrame(proj->getMovie()->getFrames().size()-1);
}

std::shared_ptr<Object> DataProvider::cellAtFrame(int frame, double x, double y) {
    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    if (!proj)
        return nullptr;

    QList<std::shared_ptr<Slice>> slices = proj->getMovie()
            ->getFrame(frame)
            ->getSlices();

    QPointF p = QPoint(x,y) / DataProvider::getInstance()->getScaleFactor();
    qreal scaledX = p.x();
    qreal scaledY = p.y();

    for (std::shared_ptr<Slice> s : slices)
        for (std::shared_ptr<Channel> c: s->getChannels().values())
            for (std::shared_ptr<Object> o : c->getObjects().values()){
                std::shared_ptr<QRect> bb = o->getBoundingBox();
                if (bb->left() <= scaledX
                        && bb->right() >= scaledX
                        && bb->top() <= scaledY
                        && bb->bottom() >= scaledY){
                    if (o->getOutline()->containsPoint(p, Qt::OddEvenFill))
                        return o;
                }
            }

    return nullptr;
}

std::shared_ptr<Object> DataProvider::cellAt(double x, double y) {
    int currentFrame = GUIState::getInstance()->getCurrentFrame();
    return cellAtFrame(currentFrame, x, y);
}

int DataProvider::cellIDAt(double x, double y) {
    std::shared_ptr<Object> o = cellAt(x,y);
    return o?static_cast<int>(o->getId()):INT_MAX;
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
