/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2017, 2016, 2015 Sebastian Wagner
 *
 * TraCurate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TraCurate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with TraCurate.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <QtConcurrent/QtConcurrent>
#include <QtDebug>

#include "dataprovider.h"
#include "messagerelay.h"
#include "guistate.h"
#include "exceptions/tcexception.h"

namespace TraCurate {

DataProvider *DataProvider::theInstance = nullptr;

DataProvider *DataProvider::getInstance(){
    if (!theInstance)
        theInstance = new DataProvider();
    return theInstance;
}

DataProvider::DataProvider(QObject *parent) :
    QObject(parent), scaleFactor(1.0) {}

qreal DataProvider::getDevicePixelRatio() const
{
    return devicePixelRatio;
}

void DataProvider::setDevicePixelRatio(const qreal &value)
{
    devicePixelRatio = value;
}

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
        if (id >= 0 && a->getId() == static_cast<uint32_t>(id)) {
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

void DataProvider::waitForFutures()
{
    for (QFuture<void> &f : futures)
        f.waitForFinished();
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
void DataProvider::runLoad(QString fileName) {
    QUrl url(fileName);
    std::shared_ptr<Project> proj = importer->load(url.toLocalFile());
    GUIState::getInstance()->setProj(proj);
    GUIState::getInstance()->setMaximumFrame(proj->getMovie()->getFrames().size()-1);
    GUIState::getInstance()->setMaximumSlice(proj->getMovie()->getFrame(0)->getSlices().size());
    GUIState::getInstance()->setMaximumChannel(proj->getMovie()->getFrame(0)->getSlice(0)->getChannels().size());
    MessageRelay::emitFinishNotification();
}

/*!
 * \brief Asynchronously calls the method to load a project from HDF5
 * \param fileName is the name of the HDF5 file
 */
void DataProvider::loadHDF5(QString fileName)
{
    importer = std::make_shared<ImportHDF5>();
    QFuture<void> f = QtConcurrent::run(this, &DataProvider::runLoad, fileName);
    futures.append(f);
}

/*!
 * \brief Asynchronously calls the method to load a project from HDF5
 * \param fileName is the name of the HDF5 file
 */
void DataProvider::loadXML(QString fileName)
{
    importer = std::make_shared<ImportXML>();
    QFuture<void> f = QtConcurrent::run(this, &DataProvider::runLoad, fileName);
    futures.append(f);
}

void DataProvider::runSaveHDF5(QString filename, Export::SaveOptions &so)
{
    QUrl url(filename);
    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    exporter.save(proj, url.toLocalFile(), so);
    MessageRelay::emitFinishNotification();
    GUIState::getInstance()->setMaximumFrame(proj->getMovie()->getFrames().size()-1);
    GUIState::getInstance()->setMaximumSlice(proj->getMovie()->getFrame(0)->getSlices().size());
    GUIState::getInstance()->setMaximumChannel(proj->getMovie()->getFrame(0)->getSlice(0)->getChannels().size());
}

/*!
 * \brief Writes the project to a HDF5 file
 * \param fileName is the name of the HDF5 file
 */
void DataProvider::runSaveHDF5(QString fileName)
{
    QUrl url(fileName);
    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    exporter.save(proj, url.toLocalFile());
    MessageRelay::emitFinishNotification();
    GUIState::getInstance()->setMaximumFrame(proj->getMovie()->getFrames().size()-1);
    GUIState::getInstance()->setMaximumSlice(proj->getMovie()->getFrame(0)->getSlices().size());
    GUIState::getInstance()->setMaximumChannel(proj->getMovie()->getFrame(0)->getSlice(0)->getChannels().size());
}

/*!
 * \brief Writes the project to the HDF5 file it was loaded from
 */
void DataProvider::runSaveHDF5()
{
    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    qDebug() << "saving to" << proj->getFileName();
    exporter.save(proj, proj->getFileName());
    qDebug() << "saving finished";
    MessageRelay::emitFinishNotification();
    GUIState::getInstance()->setMaximumFrame(proj->getMovie()->getFrames().size()-1);
    GUIState::getInstance()->setMaximumSlice(proj->getMovie()->getFrame(0)->getSlices().size());
    GUIState::getInstance()->setMaximumChannel(proj->getMovie()->getFrame(0)->getSlice(0)->getChannels().size());
}

void DataProvider::saveHDF5(QString filename, bool sAnnotations, bool sAutoTracklets, bool sEvents, bool sImages, bool sInfo, bool sObjects, bool sTracklets)
{
    Export::SaveOptions so{sAnnotations, sAutoTracklets, sEvents, sImages, sInfo, sObjects, sTracklets};

    QFuture<void> f = QtConcurrent::run(this, &DataProvider::runSaveHDF5, filename, so);
    futures.append(f);
}

void DataProvider::saveHDF5(QString fileName)
{
    QFuture<void> f = QtConcurrent::run(this, &DataProvider::runSaveHDF5, fileName);
    futures.append(f);
}

void DataProvider::saveHDF5()
{
    QFuture<void> f = QtConcurrent::run(this, &DataProvider::runSaveHDF5);
    futures.append(f);
}

bool DataProvider::sanityCheckOptions(QString filename, bool sAnnotations, bool sAutoTracklets, bool sEvents, bool sImages, bool sInfo, bool sObjects, bool sTracklets)
{
    try {
        Export::SaveOptions so{sAnnotations, sAutoTracklets, sEvents, sImages, sInfo, sObjects, sTracklets};
        bool valid = exporter.sanityCheckOptions(GUIState::getInstance()->getProj(), filename, so);
        MessageRelay::emitUpdateStatusBar("");
        return valid;
    } catch (TCException &e) {
        MessageRelay::emitUpdateStatusBar(e.what());
        return false;
    }
}

void DataProvider::runImportFiji(Project::XMLProjectSpec xps) {
    std::shared_ptr<ImportXML> ix = std::dynamic_pointer_cast<ImportXML>(importer);
    std::shared_ptr<Project> proj = ix->load(xps);
    proj->setProjectSpec(xps);
    GUIState::getInstance()->setProj(proj);
    GUIState::getInstance()->setMaximumFrame(proj->getMovie()->getFrames().size()-1);
    GUIState::getInstance()->setMaximumSlice(proj->getMovie()->getFrame(0)->getSlices().size());
    GUIState::getInstance()->setMaximumChannel(proj->getMovie()->getFrame(0)->getSlice(0)->getChannels().size());
    MessageRelay::emitFinishNotification();
}

void DataProvider::importFiji(QJSValue data)
{
    Project::XMLProjectSpec p;

    if (data.hasProperty("projectFile"))
        p.projectFile = data.property("projectFile").toString();
    if (data.hasProperty("rows"))
        p.rows = data.property("rows").toInt();
    if (data.hasProperty("cols"))
        p.cols = data.property("cols").toInt();
    if (data.hasProperty("slices")) {
        QJSValue slices = data.property("slices");
        for (int i = 0; i < slices.property("length").toInt(); i++) {
            Project::XMLSliceSpec s;
            QJSValue slice = slices.property(i);
            if (slice.hasProperty("tracks"))
                s.tracks = slice.property("tracks").toString();
            if (slice.hasProperty("xml"))
                s.xml = slice.property("xml").toString();
            if (slice.hasProperty("channels")) {
                QJSValue channels = slice.property("channels");
                for (int j = 0; j < channels.property("length").toInt(); j++) {
                    QString channel = channels.property(j).toString();
                    s.channels.push_back(channel);
                }
            }
            p.slices.push_back(s);
        }
    }

    importer = std::make_shared<ImportXML>();
    QtConcurrent::run(this, &DataProvider::runImportFiji, p);
}

QString DataProvider::localFileFromURL(QString path)
{
    QUrl u(path);
    return u.toLocalFile();
}

/*!
 * \brief returns the Object at Position (x,y) in a given Frame/Slice/Channel or nullptr
 * \param frame the frame to inspect
 * \param slice the slice to inspect
 * \param channel the channel to inspect
 * \param x the x-coordinate to inspect
 * \param y the y-coordinate to inspect
 * \return the Object or nullptr if there is none
 */
std::shared_ptr<Object> DataProvider::cellAtFrame(int frame, int slice, int channel, double x, double y) {
    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    if (!proj)
        return nullptr;

    std::shared_ptr<Movie> movie = proj->getMovie();
    if (!movie)
        return nullptr;

    std::shared_ptr<Frame> f = movie->getFrame(frame);
    if (!f)
        return nullptr;

    std::shared_ptr<Slice> s = f->getSlice(slice);
    if (!s)
        return nullptr;

    std::shared_ptr<Channel> c = s->getChannel(channel);
    if (!c)
        return nullptr;

    QPointF p = QPointF(x,y) / DataProvider::getInstance()->getScaleFactor();
    qreal scaledX = p.x();
    qreal scaledY = p.y();


    for (std::shared_ptr<Object> o : c->getObjects().values()){
        std::shared_ptr<QPolygonF> poly = o->getOutline();
        if (poly->boundingRect().contains(scaledX, scaledY))
            if (poly->containsPoint(p, Qt::OddEvenFill))
                return o;
    }

    return nullptr;
}

/*!
 * \brief wrapper for cellAtFrame that uses the current Frame/Slice/Channel
 * \param x the x-coordinate to inspect
 * \param y the y-coordinate to inspect
 * \return the Object or nullptr if there is none
 */
std::shared_ptr<Object> DataProvider::cellAt(double x, double y) {
    int currentFrame = GUIState::getInstance()->getCurrentFrame();
    int currentSlice = GUIState::getInstance()->getCurrentSlice();
    int currentChannel = GUIState::getInstance()->getCurrentChannel();
    return cellAtFrame(currentFrame, currentSlice, currentChannel, x, y);
}

/*!
 * \brief returns the ID of the Cell at position (x,y) in the current Frame/Slice/Channel
 * \param x the x-coordinate to inspect
 * \param y the y-coordinate to inspect
 * \return the id of the Object at (x,y) or INT_MAX if there is none
 */
int DataProvider::cellIDAt(double x, double y) {
    std::shared_ptr<Object> o = cellAt(x,y);
    return o?static_cast<int>(o->getId()):INT_MAX;
}

/*!
 * \brief Returns an image of an HDF5 file.
 * \param fileName is the name of the HDF5 file
 * \param frameNumber is the number of the Frame
 * \param sliceNumber is the number of the Slice
 * \param channelNumber is the number of the Channel
 * \return the requested QImage
 */
QImage DataProvider::requestImage(QString fileName, int frameNumber, int sliceNumber, int channelNumber)
{
    QUrl url(fileName);
    std::shared_ptr<QImage> img;
    img = importer->requestImage(url.toLocalFile(), frameNumber, sliceNumber, channelNumber);
    return *img.get();
}

}
