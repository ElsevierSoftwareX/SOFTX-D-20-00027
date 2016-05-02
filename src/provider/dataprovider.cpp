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

DataProvider::DataProvider(QObject *parent) :
    QObject(parent) {}

void DataProvider::setAnnotations(const QList<QObject *> &value)
{
    if (annotations != value)
        emit annotationsChanged(annotations = value);
}

/*!
 * \brief creates a new Annotation
 * \param t the type of the Annotation
 * \return the ID of the newly created Annotation
 */
int DataProvider::addAnnotation(int t)
{
    Annotation::ANNOTATION_TYPE type = static_cast<Annotation::ANNOTATION_TYPE>(t);
    auto a = std::make_shared<Annotation>(type);
    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    if (!proj)
        return -1;
    std::shared_ptr<Genealogy> gen = proj->getGenealogy();
    if (!gen)
        return -1;
    gen->addAnnotation(a);
    emit annotationsChanged(annotations);
    return a->getId();
}

/*!
 * \brief changes an Annotation%s type, title and description
 * \param id the ID of the Annotation to change
 * \param t the type of the Annotation
 * \param title the title of the Annotation
 * \param description the description of the Annotation
 */
void DataProvider::changeAnnotation(int id, int t, QString title, QString description)
{
    Annotation::ANNOTATION_TYPE type = static_cast<Annotation::ANNOTATION_TYPE>(t);
    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    if (!proj)
        return;
    std::shared_ptr<Genealogy> gen = proj->getGenealogy();
    if (!gen)
        return;
    std::shared_ptr<QList<std::shared_ptr<Annotation>>> anno = gen->getAnnotations();
    for (std::shared_ptr<Annotation> a : *anno)
        if (id >= 0 && a->getId() == (uint32_t)id) {
            bool changed = (a->getType() != type) || (a->getTitle() != title) || (a->getDescription() != description);
            a->setType(type);
            a->setTitle(title);
            a->setDescription(description);
            if (changed)
                emit annotationsChanged(annotations);
        }
}

/*!
 * \brief deletes a Annotation
 * \param id the ID of the Annotation to delete
 */
void DataProvider::deleteAnnotation(int id)
{
    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    if (!proj)
        return;
    std::shared_ptr<Genealogy> gen = proj->getGenealogy();
    if (!gen)
        return;
    std::shared_ptr<Annotation> annotation = gen->getAnnotation(id);
    gen->deleteAnnotation(annotation);
    /* no need to change this model, as the signal is emitted and anything
     * using annotations should call getAnnotations() after that */
    emit annotationsChanged(annotations);
}

/*!
 * \brief checks, if the selected Object/Tracklet is annotated with the Annotation that has the given ID
 * \param annotationId the ID of the Annotation to check
 * \return true, if the selected Object/Tracklet is annotated with the Annotation, false otherwise
 */
bool DataProvider::isAnnotatedWith(int annotationId)
{
    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    if (!proj)
        return false;
    std::shared_ptr<Genealogy> gen = proj->getGenealogy();
    if (!gen)
        return false;
    std::shared_ptr<Annotation> annotation = gen->getAnnotation(annotationId);
    if (!annotation)
        return false;
    std::shared_ptr<Annotateable> annotated;
    switch (annotation->getType()) {
    case Annotation::ANNOTATION_TYPE::OBJECT_ANNOTATION: {
        annotated = GUIState::getInstance()->getSelectedCell().lock();
        break; }
    case Annotation::ANNOTATION_TYPE::TRACKLET_ANNOTATION: {
        annotated = GUIState::getInstance()->getSelectedTrack().lock();
        break; }
    }
    return annotated && annotated->isAnnotatedWith(annotation);
}

/*!
 * \brief toggles, if the currently selected Object/Tracklet is annotated with the Annotation that has the given ID
 * \param annotationId the ID of the Annotation to toggle
 */
void DataProvider::toggleAnnotate(int annotationId)
{
    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    if (!proj)
        return;
    std::shared_ptr<Genealogy> gen = proj->getGenealogy();
    if (!gen)
        return;
    std::shared_ptr<Annotation> annotation = gen->getAnnotation(annotationId);
    if (!annotation)
        return;
    std::shared_ptr<Annotateable> annotated;
    switch (annotation->getType()) {
    case Annotation::ANNOTATION_TYPE::OBJECT_ANNOTATION:
        annotated = GUIState::getInstance()->getSelectedCell().lock();
        break;
    case Annotation::ANNOTATION_TYPE::TRACKLET_ANNOTATION:
        annotated = GUIState::getInstance()->getSelectedTrack().lock();
        break;
    }
    if (!annotated)
        return;
    if (annotated->isAnnotatedWith(annotation))
        gen->unannotate(annotated, annotation);
    else
        gen->annotate(annotated, annotation);
    annotationsChanged(annotations);
}

/*!
 * \brief annotates the selected Object with the Annotation that has the given ID
 * \param id the ID of the Annotation
 */
void DataProvider::annotateSelectedObject(int id)
{
    std::shared_ptr<Genealogy> gen = GUIState::getInstance()->getProj()->getGenealogy();
    std::shared_ptr<Object> currentObject = GUIState::getInstance()->getSelectedCell().lock();
    std::shared_ptr<Annotation> annotation = gen->getAnnotation(id);

    if (gen && currentObject && annotation)
        GUIState::getInstance()->getProj()->getGenealogy()->annotate(currentObject,annotation);
    annotationsChanged(annotations);
}

/*!
 * \brief annotates the selected Tracklet with the Annotation that has the given ID
 * \param id the ID of the Annotation
 */
void DataProvider::annotateSelectedTracklet(int id)
{
    std::shared_ptr<Genealogy> gen = GUIState::getInstance()->getProj()->getGenealogy();
    std::shared_ptr<Tracklet> currentTracklet = GUIState::getInstance()->getSelectedTrack().lock();
    std::shared_ptr<Annotation> annotation = gen->getAnnotation(id);

    if (gen && currentTracklet && annotation)
        GUIState::getInstance()->getProj()->getGenealogy()->annotate(currentTracklet,annotation);
    annotationsChanged(annotations);
}

/*!
 * \brief returns all Annotations for usage in QML
 * \return a QList of pointers to Annotations for use in QML
 */
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

/*!
 * \brief returns the current scaleFactor
 * \return the current scaleFactor
 */
double DataProvider::getScaleFactor() const
{
    return scaleFactor;
}

/*!
 * \brief sets the current scaleFactor
 * \param value the new scaleFactor
 */
void DataProvider::setScaleFactor(double value)
{
    scaleFactor = value;
}

QList<QObject *> DataProvider::getTracklets()
{
    QList<QObject*> old = tracklets;
    QList<QObject*> ret;

    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    if (!proj)
        return ret;
    std::shared_ptr<Genealogy> gen = proj->getGenealogy();
    if (!gen)
        return ret;
    std::shared_ptr<QHash<int, std::shared_ptr<Tracklet>>> th = gen->getTracklets();
    if (!th)
        return ret;
    QList<std::shared_ptr<Tracklet>> ts = th->values();

    for (std::shared_ptr<Tracklet> t : ts)
        ret.push_back(t.get());

    /* check if changed */
    if (old.length() == ret.length()) {
        bool changed = false;
        for (int i = 0; i < old.length(); i++)
            if (old[i] != ret[i])
                changed = true;
        if (!changed)
            return old;
    }
    emit trackletsChanged(tracklets = ret);
    return ret;
}

void DataProvider::setTracklets(const QList<QObject *> &value)
{
    if (tracklets != value)
        emit trackletsChanged(tracklets = value);
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

/*!
 * \brief Writes the project to the HDF5 file it was loaded from
 */
void DataProvider::saveHDF5()
{
    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    qDebug() << "saving to" << proj->getFileName();
    exporter.save(proj, proj->getFileName());
    MessageRelay::emitFinishNotification();
    GUIState::getInstance()->setMaximumFrame(proj->getMovie()->getFrames().size()-1);
}

QString DataProvider::localFileFromURL(QString path)
{
    QUrl u(path);
    return u.toLocalFile();
}

/*!
 * \brief returns the Object at Position (x,y) in a given Frame or nullptr
 * \param frame
 * \param x
 * \param y
 * \return the Object or nullptr if there is none
 */
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
                if (bb->contains(scaledX, scaledY, false))
                    if (o->getOutline()->containsPoint(p, Qt::OddEvenFill))
                        return o;
            }

    return nullptr;
}

/*!
 * \brief wrapper for cellAtFrame that uses the current Frame
 * \param x
 * \param y
 * \return
 */
std::shared_ptr<Object> DataProvider::cellAt(double x, double y) {
    int currentFrame = GUIState::getInstance()->getCurrentFrame();
    return cellAtFrame(currentFrame, x, y);
}

/*!
 * \brief returns the ID of the Cell at position (x,y) in the current Frame
 * \param x
 * \param y
 * \return
 */
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
