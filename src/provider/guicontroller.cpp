#include "guicontroller.h"

#include <QtConcurrent/QtConcurrent>
#include <QDebug>

#include "guistate.h"
#include "graphics/base.h"
#include "graphics/merge.h"
#include "graphics/separate.h"
#include "graphics/floodfill.h"
#include "exceptions/ctunimplementedexception.h"
#include "provider/imageprovider.h"
#include "tracked/trackevent.h"
#include "tracked/trackeventdivision.hpp"
#include "tracked/trackeventdead.hpp"
#include "tracked/trackeventendofmovie.hpp"
#include "tracked/trackeventlost.hpp"
#include "tracked/trackeventmerge.hpp"
#include "tracked/trackeventunmerge.hpp"
#include "io/modifyhdf5.h"

namespace CellTracker {

GUIController *GUIController::theInstance = nullptr;

GUIController::GUIController(QObject *parent) :
    QObject(parent),
    abortStrategyIssued(false),
    currentStrategy(GUIState::Strategy::STRATEGY_DEFAULT),
    currentStrategyRunning(false),
    currentAction(GUIState::Action::ACTION_DEFAULT) {}

/*!
 * \brief returns the instance of GUIController
 * \return the instance of GUIController
 */
GUIController *GUIController::getInstance() {
    if (!theInstance)
        theInstance = new GUIController();
    return theInstance;
}

/*!
 * \brief provides the instance of GUIController for QML
 * \param engine (unused)
 * \param scriptEngine (unused)
 * \return a pointer to the instance of GUIController
 */
QObject *GUIController::qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return getInstance();
}

void GUIController::waitForFutures()
{
    for (QFuture<void> &f : futures)
        f.waitForFinished();
}

/*!
 * \brief changes the current Frame to a specific value
 * \param newFrame the number of the new Frame
 */
void GUIController::changeFrameAbs(int newFrame) {
    GUIState::getInstance()->setCurrentFrame(newFrame);
    QObject *s = GUIState::getInstance()->getSlider();
    if (s) s->setProperty("value", newFrame);
}

void GUIController::changeSlice(int newSlice) {
    GUIState::getInstance()->setCurrentSlice(newSlice);
    emit GUIState::getInstance()->backingDataChanged();
}

void GUIController::changeChannel(int newChannel) {
    GUIState::getInstance()->setCurrentChannel(newChannel);
    emit GUIState::getInstance()->backingDataChanged();
}

/*!
 * \brief changes the current Frame by a relative value
 * \param diff the difference to the current frame
 */
void GUIController::changeFrame(int diff) {
    int curr = GUIState::getInstance()->getCurrentFrame();
    int nVal;
    if (curr + diff < 0)
        nVal = 0;
    else if (curr + diff > GUIState::getInstance()->getMaximumFrame())
        nVal = GUIState::getInstance()->getMaximumFrame();
    else
        nVal = curr + diff;

    GUIState::getInstance()->setCurrentFrame(nVal);
    QObject *s = GUIState::getInstance()->getSlider();
    if (s) s->setProperty("value", nVal);
}

/*!
 * \brief sets an Object as hovered
 * \param o the Object to set hovered
 */
void GUIController::hoverCell(std::shared_ptr<Object> const &o) {
    GUIState::getInstance()->setHoveredCell(o);
    GUIState::getInstance()->setHoveredCellID(o->getId());
    GUIState::getInstance()->sethoveredCellFrame(o->getFrameId());
}

/*!
 * \brief resets the hovered Object
 */
void GUIController::unhoverCell() {
    GUIState::getInstance()->setHoveredCell(std::weak_ptr<Object>());
    GUIState::getInstance()->setHoveredCellID(-1);
    GUIState::getInstance()->sethoveredCellFrame(-1);
}

/*!
 * \brief sets the Tracklet of an Object as hovered
 * \param o the Object whose Tracklet should be set hovered
 * \param proj the current Project
 */
void GUIController::hoverTrack(std::shared_ptr<Object> const &o, std::shared_ptr<Project> const &proj) {
    if (!o || !proj)
        return;
    std::shared_ptr<Tracklet> t = proj->getGenealogy()->getTracklet(o->getTrackId());
    if (!t)
        return;
    GUIState::getInstance()->setHoveredTrackID(t->getId());
    uint32_t start = t->getStart().first->getID();
    uint32_t end = t->getEnd().first->getID();
    uint32_t length = end - start;
    QString status = t->qmlStatus();
    QString previous = t->qmlPrevious();
    QString next = t->qmlNext();

    GUIState::getInstance()->setHoveredTrackStart(start);
    GUIState::getInstance()->setHoveredTrackEnd(end);
    GUIState::getInstance()->setHoveredTrackLength(length);
    GUIState::getInstance()->setHoveredTrackStatus(status);
    GUIState::getInstance()->setHoveredTrackPrevious(previous);
    GUIState::getInstance()->setHoveredTrackNext(next);
}

/*!
 * \brief resets the hovered Tracklet
 */
void GUIController::unhoverTrack() {
    GUIState::getInstance()->setHoveredTrackID(-1);
    GUIState::getInstance()->setHoveredTrackStart(-1);
    GUIState::getInstance()->setHoveredTrackEnd(-1);
    GUIState::getInstance()->setHoveredTrackLength(-1);
    GUIState::getInstance()->setHoveredTrackStatus("");
    GUIState::getInstance()->setHoveredTrackPrevious("");
    GUIState::getInstance()->setHoveredTrackNext("");
}

/*!
 * \brief sets the AutoTracklet of an Object as hovered
 * \param o the Object whose AutoTracklet should be set hovered
 * \param proj the current Project
 */
void GUIController::hoverAutoTracklet(std::shared_ptr<Object> const &o, std::shared_ptr<Project> const &proj) {
    std::shared_ptr<AutoTracklet> at = proj->getAutoTracklet(o->getAutoId());
    GUIState::getInstance()->setHoveredAutoTrackID(at->getID());
    uint32_t start = at->getStart();
    uint32_t end = at->getEnd();
    uint32_t length = at->getLength();

    GUIState::getInstance()->setHoveredAutoTrackStart(start);
    GUIState::getInstance()->setHoveredAutoTrackEnd(end);
    GUIState::getInstance()->setHoveredAutoTrackLength(length);
}

/*!
 * \brief resets the hovered AutoTracklet
 */
void GUIController::unhoverAutoTracklet() {
    GUIState::getInstance()->setHoveredAutoTrackID(-1);
    GUIState::getInstance()->setHoveredAutoTrackStart(-1);
    GUIState::getInstance()->setHoveredAutoTrackEnd(-1);
    GUIState::getInstance()->setHoveredAutoTrackLength(-1);
}

/*!
 * \brief sets the Object at position (x,y) to hovered if there is one
 * \param frame the current Frame
 * \param slice the current Slice
 * \param channel the current Channel
 * \param x the x-position to hover
 * \param y the y-position to hover
 */
void GUIController::hoverCell(int frame, int slice, int channel, double x, double y){
    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    std::shared_ptr<Object> o = DataProvider::getInstance()->cellAtFrame(frame, slice, channel, x, y);

    if (!proj) /* we don't have a project yet */
        return;

    if (!o) { /* there is no cell at this position */
        unhoverCell();
        unhoverTrack();
        unhoverAutoTracklet();
        return;
    }

    hoverCell(o);

    if (o->isInTracklet())
        hoverTrack(o, proj);
    else
        unhoverTrack();

    if (o->isInAutoTracklet())
        hoverAutoTracklet(o, proj);
    else
        unhoverAutoTracklet();
}

/*!
 * \brief sets an Object as selected
 * \param o the Object to set selected
 */
void GUIController::selectCell(std::shared_ptr<Object> const &o) {
    GUIState::getInstance()->setSelectedCell(o);
    GUIState::getInstance()->setSelectedCellID(o->getId());
    GUIState::getInstance()->setSelectedCellFrame(o->getFrameId());
}

/*!
 * \brief resets the selected Object
 */
void GUIController::deselectCell() {
    GUIState::getInstance()->setSelectedCell(std::weak_ptr<Object>());
    GUIState::getInstance()->setSelectedCellID(-1);
    GUIState::getInstance()->setSelectedCellFrame(-1);
}

/*!
 * \brief sets the Tracklet of an Object as selected
 * \param o the Object whose Tracklet should be set selected
 * \param proj the current Project
 */
void GUIController::selectTrack(std::shared_ptr<Object> const &o, std::shared_ptr<Project> const &proj) {
    if (!o || !proj)
        return;
    std::shared_ptr<Tracklet> t = proj->getGenealogy()->getTracklet(o->getTrackId());
    if (!t)
        return;
    GUIState::getInstance()->setSelectedTrackID(t->getId());
    uint32_t start = t->getStart().first->getID();
    uint32_t end = t->getEnd().first->getID();
    uint32_t length = end - start;
    QString status = t->qmlStatus();
    QString previous = t->qmlPrevious();
    QString next = t->qmlNext();

    GUIState::getInstance()->setSelectedTrackStart(start);
    GUIState::getInstance()->setSelectedTrackEnd(end);
    GUIState::getInstance()->setSelectedTrackLength(length);
    GUIState::getInstance()->setSelectedTrackStatus(status);
    GUIState::getInstance()->setSelectedTrackPrevious(previous);
    GUIState::getInstance()->setSelectedTrackNext(next);
}

/*!
 * \brief resets the selected Tracklet
 */
void GUIController::deselectTrack() {
    GUIState::getInstance()->setSelectedTrackID(-1);
    GUIState::getInstance()->setSelectedTrackStart(-1);
    GUIState::getInstance()->setSelectedTrackEnd(-1);
    GUIState::getInstance()->setSelectedTrackLength(-1);
    GUIState::getInstance()->setSelectedTrackStatus("");
    GUIState::getInstance()->setSelectedTrackPrevious("");
    GUIState::getInstance()->setSelectedTrackNext("");
}

/*!
 * \brief sets the AutoTracklet of an Object as selected
 * \param o the Object whose AutoTracklet should be set selected
 * \param proj the current Project
 */
void GUIController::selectAutoTracklet(std::shared_ptr<Object> const &o, std::shared_ptr<Project> const &proj) {
    std::shared_ptr<AutoTracklet> at = proj->getAutoTracklet(o->getAutoId());
    GUIState::getInstance()->setSelectedAutoTrackID(at->getID());
    uint32_t start = at->getStart();
    uint32_t end = at->getEnd();
    uint32_t length = at->getLength();

    GUIState::getInstance()->setSelectedAutoTrackStart(start);
    GUIState::getInstance()->setSelectedAutoTrackEnd(end);
    GUIState::getInstance()->setSelectedAutoTrackLength(length);
}

/*!
 * \brief resets the selected AutoTracklet
 */
void GUIController::deselectAutoTracklet() {
    GUIState::getInstance()->setSelectedAutoTrackID(-1);
    GUIState::getInstance()->setSelectedAutoTrackStart(-1);
    GUIState::getInstance()->setSelectedAutoTrackEnd(-1);
    GUIState::getInstance()->setSelectedAutoTrackLength(-1);
}

/*!
 * \brief sets the Object at position (x,y) to selected if there is one
 * \param frame the current Frame
 * \param slice the current Slice
 * \param channel the current Channel
 * \param x the x-position to select
 * \param y the y-position to select
 *
 * Depending on the currently selected Action, this might perform those Actions
 */
void GUIController::selectCell(int frame, int slice, int channel, double x, double y){
    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    std::shared_ptr<Object> o = DataProvider::getInstance()->cellAtFrame(frame, slice, channel, x, y);

    if (!proj) /* we don't have a project yet */
        return;

    if (!o && currentAction == GUIState::Action::ACTION_DEFAULT) { /* only do this when action is default */
        deselectCell();
        deselectTrack();
        deselectAutoTracklet();
        return;
    }

    if (!o && currentAction == GUIState::Action::ACTION_DEFAULT) { /* only do this when action is default */
        deselectCell();
        deselectTrack();
        deselectAutoTracklet();
        return;
    }

    switch (currentAction) {
    case GUIState::ACTION_DEFAULT:
    {
        selectCell(o);

        if (o->isInTracklet())
            selectTrack(o, proj);
        else
            deselectTrack();

        if (o->isInAutoTracklet())
            selectAutoTracklet(o, proj);
        else
            deselectAutoTracklet();

        break;
    }
    case GUIState::ACTION_ADD_DAUGHTERS:
    {
        std::shared_ptr<Object> mother, daughter;
        std::shared_ptr<Genealogy> gen = proj->getGenealogy();

        mother = GUIState::getInstance()->getSelectedCell().lock();
        daughter = o;

        if (!mother || !daughter)
            return;

        if (mother->getFrameId() > daughter->getFrameId())
            return;

        std::shared_ptr<Tracklet> motherT;
        motherT = gen->getTracklet(mother->getTrackId());

        if (!motherT)
            return;

        if (gen->hasDaughterObject(motherT, daughter))
            gen->removeDaughterTrack(motherT, daughter);
        else
            gen->addDaughterTrack(motherT, daughter);

        emit GUIState::getInstance()->backingDataChanged();
        break;
    }
    case GUIState::Action::ACTION_ADD_MERGER:
    {
        std::shared_ptr<Object> merged, unmerged;
        std::shared_ptr<Genealogy> gen = proj->getGenealogy();

        merged = GUIState::getInstance()->getSelectedCell().lock();
        unmerged = o;

        if (!merged || !unmerged)
            return;

        if (merged->getFrameId() < unmerged->getFrameId())
            return;

        std::shared_ptr<Tracklet> unmergedT;
        unmergedT = gen->getTracklet(unmerged->getTrackId());

        if (!unmergedT)
            return;

        if (gen->hasMergerObject(unmergedT, merged))
            gen->removeMergedTrack(unmergedT, merged);
        else
            gen->addMergedTrack(unmergedT, merged);

        emit GUIState::getInstance()->backingDataChanged();
        break;
    }
    case GUIState::Action::ACTION_ADD_UNMERGER:
    {
        std::shared_ptr<Object> merged, unmerged;
        std::shared_ptr<Genealogy> gen = proj->getGenealogy();

        merged = GUIState::getInstance()->getSelectedCell().lock();
        unmerged = o;

        if (!merged || !unmerged)
            return;

        if (merged->getFrameId() > unmerged->getFrameId())
            return;

        std::shared_ptr<Tracklet> mergedT;
        mergedT = gen->getTracklet(merged->getTrackId());

        if (!mergedT)
            return;

        if (gen->hasUnmergerObject(mergedT, unmerged))
            gen->removeUnmergedTrack(mergedT, unmerged);
        else
            gen->addUnmergedTrack(mergedT, unmerged);

        emit GUIState::getInstance()->backingDataChanged();
        break;
    }
    case GUIState::ACTION_DELETE_CELL:
    {
        std::shared_ptr<Object> cell;
        int currentFrame;

        cell = o;
        currentFrame = GUIState::getInstance()->getCurrentFrame();

        if (!cell) /* no cell selected */
            return;

        std::shared_ptr<Tracklet> t = proj->getGenealogy()->getTracklet(cell->getTrackId());

        if (!t)
            return;

        t->removeFromContained(currentFrame, cell->getId());

        if (t->getContained().isEmpty()) /* remove tracklet if there are no more cells in it */
            proj->getGenealogy()->removeTracklet(t->getId());

        emit GUIState::getInstance()->backingDataChanged();
        break;
    }
    case GUIState::ACTION_DELETE_CELLS_FROM:
    {
        std::shared_ptr<Object> cell;
        int currentFrame;

        cell = o;
        currentFrame = GUIState::getInstance()->getCurrentFrame();

        if (!cell) /* no cell selected */
            return;

        std::shared_ptr<Tracklet> t = proj->getGenealogy()->getTracklet(cell->getTrackId());

        if (!t)
            return;

        auto contained = t->getContained();
        for (int key: contained.keys()) {
            auto val = contained.value(key);
            if (currentFrame >= 0 && val.first->getID() >= static_cast<uint32_t>(currentFrame))
                t->removeFromContained(val.first->getID(), val.second->getId());
        }

        if (t->getContained().isEmpty()) /* remove tracklet if there are no more cells in it */
            proj->getGenealogy()->removeTracklet(t->getId());

        GUIState::getInstance()->backingDataChanged();
        break;
    }
    case GUIState::ACTION_DELETE_CELLS_TILL:
    {
        std::shared_ptr<Object> cell;
        int currentFrame;

        cell = o;
        currentFrame = GUIState::getInstance()->getCurrentFrame();

        if (!cell) /* no cell selected */
            return;

        std::shared_ptr<Tracklet> t = proj->getGenealogy()->getTracklet(cell->getTrackId());

        if (!t)
            return;

        auto contained = t->getContained();
        for (int key: contained.keys()) {
            auto val = contained.value(key);
            if (currentFrame >= 0 && val.first->getID() <= static_cast<uint32_t>(currentFrame))
                t->removeFromContained(val.first->getID(), val.second->getId());
        }

        if (t->getContained().isEmpty()) /* remove tracklet if there are no more cells in it */
            proj->getGenealogy()->removeTracklet(t->getId());

        GUIState::getInstance()->backingDataChanged();
        break;
    }
    }
}

void GUIController::selectLastCellByTrackId(int trackId)
{
    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    if (!proj)
        return;
    std::shared_ptr<Genealogy> gen = proj->getGenealogy();
    if (!gen)
        return;
    std::shared_ptr<Tracklet> t = gen->getTracklet(trackId);
    if (!t)
        return;

    std::shared_ptr<Object> o = t->getEnd().second;

    selectCell(o);
    if (o->isInTracklet())
        selectTrack(o, proj);
    else
        deselectTrack();

    if (o->isInAutoTracklet())
        selectAutoTracklet(o, proj);
    else
        deselectAutoTracklet();

    return;
}

void GUIController::changeStatus(int trackId, int status)
{
    std::shared_ptr<Project> p = GUIState::getInstance()->getProj();
    if (!p)
        return;

    std::shared_ptr<Genealogy> g = p->getGenealogy();
    if (!g)
        return;

    std::shared_ptr<Tracklet> t = g->getTracklet(trackId);
    if (!t)
        return;

    if (status == -2) /* unimplemented */
        return;

    TrackEvent<Tracklet>::EVENT_TYPE newTEType = static_cast<TrackEvent<Tracklet>::EVENT_TYPE>(status);
    if (t->getNext()) {
        /* remove the old Event and all references to is (meaning also that of following tracklets back to it) */
        TrackEvent<Tracklet>::EVENT_TYPE oldTEType = t->getNext()->getType();

        /* first look at the event and if there might be backreferences to this event, delete them */
        switch (oldTEType) {
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_DEAD: /* fallthrough */
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_LOST: /* fallthrough */
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_ENDOFMOVIE:
            /* all good, nothing to do */
            break;
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_DIVISION: {
            std::shared_ptr<TrackEventDivision<Tracklet>> ted = std::static_pointer_cast<TrackEventDivision<Tracklet>>(t->getNext());
            for (std::weak_ptr<Tracklet> n : *ted->getNext())
                n.lock()->setPrev(nullptr);
            break; }
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_MERGE: {
            std::shared_ptr<TrackEventMerge<Tracklet>> tem = std::static_pointer_cast<TrackEventMerge<Tracklet>>(t->getNext());
            if (tem->getPrev()->count() == 1 && tem->getPrev()->first().lock() == t) /* only this tracklet as previous */
                tem->getNext().lock()->setPrev(nullptr);
            QMutableListIterator<std::weak_ptr<Tracklet>> it(*tem->getPrev());
            while (it.hasNext())
                if (it.next().lock() == t)
                    it.remove();
            break; }
        case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_UNMERGE: {
            std::shared_ptr<TrackEventUnmerge<Tracklet>> teu = std::static_pointer_cast<TrackEventUnmerge<Tracklet>>(t->getNext());
            for (std::weak_ptr<Tracklet> n : *teu->getNext())
                n.lock()->setPrev(nullptr);
            break; }
        }

        /* now set the next reference of this tracklet to nullptr (aka "open") */
        t->setNext(nullptr);
    }

    /* set the TrackEvent of this tracklet to the new type */
    switch (newTEType) {
    case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_DIVISION:
    case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_MERGE:
    case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_UNMERGE:
        qDebug() << "track event {division, merge, unmerge} should be set by other means";
        return;
    case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_DEAD: {
        auto ted = std::make_shared<TrackEventDead<Tracklet>>();
        ted->setPrev(t);
        t->setNext(ted);
        break; }
    case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_LOST: {
        auto tel = std::make_shared<TrackEventLost<Tracklet>>();
        tel->setPrev(t);
        t->setNext(tel);
        break; }
    case TrackEvent<Tracklet>::EVENT_TYPE::EVENT_TYPE_ENDOFMOVIE: {
        auto teeom = std::make_shared<TrackEventEndOfMovie<Tracklet>>();
        teeom->setPrev(t);
        t->setNext(teeom);
        break; }
    }
    selectTrack(GUIState::getInstance()->getSelectedCell().lock(), GUIState::getInstance()->getProj());
    hoverTrack(GUIState::getInstance()->getHoveredCell().lock(), GUIState::getInstance()->getProj());
    emit GUIState::getInstance()->backingDataChanged();
}

void GUIController::cutObject(int startX, int startY, int endX, int endY)
{
    QPointF start = QPoint(startX, startY);
    QPointF end = QPoint(endX, endY);
    QLineF cutLine(start, end);
    std::shared_ptr<Object> cuttee = Base::objectCutByLine(cutLine);
    if (!cuttee) {
        qDebug() << "could not find object to cut";
        return;
    }

    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    std::shared_ptr<Movie> mov = proj->getMovie();
    std::shared_ptr<Frame> frame  = mov->getFrame(cuttee->getFrameId());
    std::shared_ptr<Slice> slice  = frame->getSlice(cuttee->getSliceId());
    std::shared_ptr<Channel> chan = slice->getChannel(cuttee->getChannelId());

    auto objects = chan->getObjects();
    auto max_obj = *std::max_element(objects.begin(), objects.end(),
                                     [](std::shared_ptr<Object> a, std::shared_ptr<Object> b){
                                                return a->getId() < b->getId();
                                     });
    int id1 = max_obj->getId() + 1;
    int id2 = max_obj->getId() + 2;
    if (id1 == INT_MAX || id2 == INT_MAX) {
        qDebug() << "Too many objects";
        return;
    }

    std::shared_ptr<Object> object1 = std::make_shared<Object>(id1, chan);
    std::shared_ptr<Object> object2 = std::make_shared<Object>(id2, chan);

    /* cut the polygon by the line and append the points of the cut outline to the newly created objects */
    double sf = DataProvider::getInstance()->getScaleFactor();
    QLineF line(start/sf, end/sf);
    QPair<QPolygonF,QPolygonF> res = Separate::compute(*cuttee->getOutline(), line);

    if (res.first.isEmpty() || res.second.isEmpty())
        return;

    auto outline1 = std::make_shared<QPolygonF>();
    for(QPointF p : res.first)
        outline1->append(p);
    object1->setOutline(outline1);
    auto outline2 = std::make_shared<QPolygonF>();
    for(QPointF p : res.second)
        outline2->append(p);
    object2->setOutline(outline2);

    auto bb1 = std::make_shared<QRect>(outline1->boundingRect().toRect());
    object1->setBoundingBox(bb1);
    auto bb2 = std::make_shared<QRect>(outline2->boundingRect().toRect());
    object2->setBoundingBox(bb2);

    /*! \todo: this is the center of the boundingBox, not the centroid of the polygon */
    auto c1 = std::make_shared<QPoint>(bb1->center());
    object1->setCentroid(c1);
    auto c2 = std::make_shared<QPoint>(bb2->center());
    object2->setCentroid(c2);

    /* replace old object in HDF5 */
    bool ret = ModifyHDF5::replaceObjects(proj->getFileName(), cuttee, {object1, object2});
    if (!ret)
        return;

    /* remove old object from autotracket/tracklet */
    if (cuttee->isInAutoTracklet()) {
        std::shared_ptr<AutoTracklet> at = proj->getAutoTracklet(cuttee->getAutoId());
        at->removeComponent(cuttee->getFrameId());
    }
    if (cuttee->isInTracklet()) {
        std::shared_ptr<Tracklet> t = proj->getGenealogy()->getTracklet(cuttee->getTrackId());
        t->removeFromContained(cuttee->getFrameId(), cuttee->getId());
    }

    /* remove the old object, add the new ones */
    chan->removeObject(cuttee->getId());
    chan->addObject(object1);
    chan->addObject(object2);

    emit GUIState::getInstance()->backingDataChanged();
}

void GUIController::mergeObjects(int firstX, int firstY, int secondX, int secondY)
{
    std::shared_ptr<Object> first = DataProvider::getInstance()->cellAt(firstX, firstY);
    std::shared_ptr<Object> second = DataProvider::getInstance()->cellAt(secondX, secondY);

    if (!first || !second) {
        qDebug() << "misclicked one of the cells";
        return;
    }
    if (first == second) {
        qDebug() << "first and second object for merge are the same";
        return;
    }

    QPolygonF outline1(*first->getOutline());
    QPolygonF outline2(*second->getOutline());
    QPolygonF merged = Merge::compute(outline1, outline2);

    if (merged.isEmpty())
        return;

    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    std::shared_ptr<Movie> mov = proj->getMovie();
    std::shared_ptr<Frame> frame = mov->getFrame(first->getFrameId());
    std::shared_ptr<Slice> slice = frame->getSlice(first->getSliceId());
    std::shared_ptr<Channel> chan = slice->getChannel(first->getChannelId());

    auto objects = chan->getObjects();
    auto max_obj = *std::max_element(objects.begin(), objects.end(),
                                   [](std::shared_ptr<Object> a, std::shared_ptr<Object> b){
                                        return a->getId() < b->getId();
                                   });
    int id = max_obj->getId() + 1;

    if (id == INT_MAX) {
        qDebug() << "Too many objects";
        return;
    }

    auto mergeObject = std::make_shared<Object>(id, chan);
    mergeObject->setOutline(std::make_shared<QPolygonF>(merged));
    mergeObject->setBoundingBox(std::make_shared<QRect>(merged.boundingRect().toRect()));
    mergeObject->setCentroid(std::make_shared<QPoint>(merged.boundingRect().center().toPoint()));

    bool ret = ModifyHDF5::replaceObjects(proj->getFileName(), {first, second}, mergeObject);
    if (!ret)
        return;

    /* remove first from autotracket/tracklet */
    if (first->isInAutoTracklet()) {
        std::shared_ptr<AutoTracklet> at = proj->getAutoTracklet(first->getAutoId());
        at->removeComponent(first->getFrameId());
    }
    if (first->isInTracklet()) {
        std::shared_ptr<Tracklet> t = proj->getGenealogy()->getTracklet(first->getTrackId());
        t->removeFromContained(first->getFrameId(), first->getId());
    }

    /* remove second from autotracket/tracklet */
    if (second->isInAutoTracklet()) {
        std::shared_ptr<AutoTracklet> at = proj->getAutoTracklet(second->getAutoId());
        at->removeComponent(second->getFrameId());
    }
    if (second->isInTracklet()) {
        std::shared_ptr<Tracklet> t = proj->getGenealogy()->getTracklet(second->getTrackId());
        t->removeFromContained(second->getFrameId(), second->getId());
    }

    chan->removeObject(first->getId());
    chan->removeObject(second->getId());
    chan->addObject(mergeObject);
    emit GUIState::getInstance()->backingDataChanged();
}

void GUIController::deleteObject(double posX, double posY)
{
    std::shared_ptr<Object> deletee = DataProvider::getInstance()->cellAt(posX, posY);

    if (!deletee) {
        qDebug() << "could not find object to delete";
        return;
    }

    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    std::shared_ptr<Movie> mov = proj->getMovie();
    std::shared_ptr<Frame> frame  = mov->getFrame(deletee->getFrameId());
    std::shared_ptr<Slice> slice  = frame->getSlice(deletee->getSliceId());
    std::shared_ptr<Channel> chan = slice->getChannel(deletee->getChannelId());

    /* delete old object in HDF5 */
    bool ret = ModifyHDF5::removeObject(proj->getFileName(), deletee);
    if (!ret)
        return;

    /* remove old object from autotracket/tracklet */
    if (deletee->isInAutoTracklet()) {
        std::shared_ptr<AutoTracklet> at = proj->getAutoTracklet(deletee->getAutoId());
        at->removeComponent(deletee->getFrameId());
    }
    if (deletee->isInTracklet()) {
        std::shared_ptr<Tracklet> t = proj->getGenealogy()->getTracklet(deletee->getTrackId());
        t->removeFromContained(deletee->getFrameId(), deletee->getId());
    }

    /* remove the old object */
    chan->removeObject(deletee->getId());

    emit GUIState::getInstance()->backingDataChanged();
}

void GUIController::floodFill(int posX, int posY)
{
    std::shared_ptr<Object> toDelete = DataProvider::getInstance()->cellAt(posX, posY);

    if (toDelete) {
        std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
        std::shared_ptr<Movie> mov = proj->getMovie();
        std::shared_ptr<Frame> frame = mov->getFrame(toDelete->getFrameId());
        std::shared_ptr<Slice> slice = frame->getSlice(toDelete->getSliceId());
        std::shared_ptr<Channel> chan = slice->getChannel(toDelete->getChannelId());

        /* delete old object in HDF5 */
        bool ret = ModifyHDF5::removeObject(proj->getFileName(), toDelete);
        if (!ret)
            return;

        if (toDelete->isInAutoTracklet()) {
            std::shared_ptr<AutoTracklet> at = proj->getAutoTracklet(toDelete->getAutoId());
            at->removeComponent(toDelete->getFrameId());
        }

        if (toDelete->isInTracklet()) {
            std::shared_ptr<Tracklet> t = proj->getGenealogy()->getTracklet(toDelete->getTrackId());
            t->removeFromContained(toDelete->getFrameId(), toDelete->getId());
        }

        /* remove the old object */
        chan->removeObject(toDelete->getId());
    }

    int fNr = GUIState::getInstance()->getCurrentFrame();
    int sNr = GUIState::getInstance()->getCurrentSlice();
    int cNr = GUIState::getInstance()->getCurrentChannel();

    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    std::shared_ptr<Movie> mov = proj->getMovie();
    std::shared_ptr<Frame> frame = mov->getFrame(fNr);
    std::shared_ptr<Slice> slice = frame->getSlice(sNr);
    std::shared_ptr<Channel> chan = slice->getChannel(cNr);

    auto objects = chan->getObjects();
    auto max_obj = *std::max_element(objects.begin(), objects.end(),
                                   [](std::shared_ptr<Object> a, std::shared_ptr<Object> b){
                                        return a->getId() < b->getId();
                                   });
    int id = max_obj->getId() + 1;

    if (id == INT_MAX) {
        qDebug() << "Too many objects";
        return;
    }

    std::shared_ptr<QImage> img;
    if (proj->getImported())
        img = ImportXML().requestImage(proj->getFileName(), fNr, sNr, cNr);
    else
        img = ImportHDF5().requestImage(proj->getFileName(), fNr, sNr, cNr);

    double sf = DataProvider::getInstance()->getScaleFactor();
    FloodFill ff(*img, 1);
    QPointF pf(posX/sf, posY/sf);
    QPoint p = pf.toPoint();
    int thresh = GUIState::getInstance()->getThresh();

    QPolygonF newOutline = ff.compute(p, thresh);
    auto newObject = std::make_shared<Object>(id, chan);
    newObject->setOutline(std::make_shared<QPolygonF>(newOutline));
    newObject->setBoundingBox(std::make_shared<QRect>(newOutline.boundingRect().toRect()));
    newObject->setCentroid(std::make_shared<QPoint>(newOutline.boundingRect().center().toPoint()));

    bool ret = ModifyHDF5::replaceObjects(proj->getFileName(), {}, newObject);
    if (!ret)
        return;

    chan->addObject(newObject);

    emit GUIState::getInstance()->backingDataChanged();
}

/*!
 * \brief returns the current strategy for QML
 * \return the strategy cast to int
 */
int GUIController::getCurrentStrategy() const {
    return static_cast<int>(currentStrategy);
}

/*!
 * \brief returns, whether currently a strategy is running
 * \return true, if a strategy is running, false if not
 */
bool GUIController::getCurrentStrategyRunning() const
{
    return currentStrategyRunning;
}

/*!
 * \brief returns the current action for QML
 * \return the action cast to int
 */
int GUIController::getCurrentAction() const {
    return static_cast<int>(currentAction);
}

/*!
 * \brief sets the current strategy from QML
 * \param value the current strategy as int
 */
void GUIController::setCurrentStrategy(int value)
{
    GUIState::Strategy newVal = static_cast<GUIState::Strategy>(value);
    if (currentStrategy != newVal)
        emit currentStrategyChanged(currentStrategy = newVal);
}

/*!
 * \brief sets, whether the current strategy is running
 * \param value the new value
 */
void GUIController::setCurrentStrategyRunning(bool value)
{
    if (currentStrategyRunning != value)
        emit currentStrategyRunningChanged(currentStrategyRunning = value);
}

/*!
 * \brief sets the current action from QML
 * \param value the current action as int
 */
void GUIController::setCurrentAction(int value)
{
    GUIState::Action newVal = static_cast<GUIState::Action>(value);
    if (value == GUIState::Action::ACTION_ADD_MERGER)
        changeFrame(-1);
    if (currentAction != newVal)
        emit currentActionChanged(currentAction = newVal);
}

/*!
 * \brief runs the strategy GUIState::Strategy::STRATEGY_CLICK_JUMP
 * \param delay the delay between frames
 * \param show how many frames to display before the end of the AutoTracklet
 *
 * For a description of strategies, see GUIController::startStrategy
 */
void GUIController::runStrategyClickJump(unsigned long delay, unsigned int show) {
    int jumpFrames;
    int displayFrames = show;

    /* get current track */
    std::shared_ptr<AutoTracklet> t = GUIState::getInstance()->getSelectedAutoTrack().lock();

    if (!t)
        goto out;

    {
    /* get length of current track */
    uint32_t start = t->getStart();
    uint32_t end = t->getEnd();

    uint32_t curr = GUIState::getInstance()->getCurrentFrame();

    if (curr < start || curr >= end) /* nothing to do, we are before or after the track */
        goto out;

    if (end - curr < show) /* we are already to near to the end to display all requested frames */
        displayFrames = end -curr;

    jumpFrames = end - displayFrames - curr;

    /* jump if jumpFrames > 0 */
    if (jumpFrames > 0)
        GUIController::getInstance()->changeFrame(jumpFrames);

    for (int i = 0; i < displayFrames; i++) {
        if (abortStrategyIssued)
            break;
        QThread::msleep(delay);
        GUIState::getInstance()->setImageReady(false);
        GUIController::getInstance()->changeFrame(1);
        /* wait until frame is actually displayed */
        while (!GUIState::getInstance()->getImageReady())
            QThread::msleep(10);
    }
    }
out:
    abortStrategyIssued = false;
    setCurrentStrategy(GUIState::Strategy::STRATEGY_DEFAULT);
    setCurrentStrategyRunning(false);
    GUIState::getInstance()->setMouseAreaActive(true);
}

/*!
 * \brief runs the strategy GUIState::Strategy::STRATEGY_CLICK_SPIN
 * \param delay the delay between frames
 *
 * For a description of strategies, see GUIController::startStrategy
 */
void GUIController::runStrategyClickSpin(unsigned long delay) {
    /* get current track */
    std::shared_ptr<AutoTracklet> t = GUIState::getInstance()->getSelectedAutoTrack().lock();

    if (!t)
        goto out;

    {
    /* get length of current track */
    uint32_t start = t->getStart();
    uint32_t end = t->getEnd();

    uint32_t begin = GUIState::getInstance()->getCurrentFrame();
    unsigned int curr = begin;

    if (begin < start || begin >= end) /* nothing to do, we are at the end of or after the track */
        goto out;

    while (true) {
        if (abortStrategyIssued)
            break;
        curr = (curr >= end)?begin:curr+1;
        QThread::msleep(delay);
        GUIState::getInstance()->setImageReady(false);
        GUIController::getInstance()->changeFrameAbs(curr);
        /* wait until frame is actually displayed */
        while (!GUIState::getInstance()->getImageReady())
            QThread::msleep(10);
    }
    }
out:
    abortStrategyIssued = false;
    setCurrentStrategy(GUIState::Strategy::STRATEGY_DEFAULT);
    setCurrentStrategyRunning(false);
    GUIState::getInstance()->setMouseAreaActive(true);
}

/*!
 * \brief runs the strategy GUIState::Strategy::STRATEGY_CLICK_STEP
 * \param delay the delay between frames
 *
 * For a description of strategies, see GUIController::startStrategy
 */
void GUIController::runStrategyClickStep(unsigned long delay) {
    /* get current track */
    std::shared_ptr<AutoTracklet> t = GUIState::getInstance()->getSelectedAutoTrack().lock();

    if (!t)
        goto out;

    {
    uint32_t end = t->getEnd();

    unsigned int curr = GUIState::getInstance()->getCurrentFrame();

    if (curr >= end) /* nothing to do, we are at the end of or after the track */
        goto out;

    for (curr = GUIState::getInstance()->getCurrentFrame(); curr < end; curr++) {
        if (abortStrategyIssued)
            break;
        QThread::msleep(delay);
        GUIState::getInstance()->setImageReady(false);
        GUIController::getInstance()->changeFrame(1);
        /* wait until frame is actually displayed */
        while (!GUIState::getInstance()->getImageReady())
            QThread::msleep(10);
    }
    }
out:
    abortStrategyIssued = false;
    setCurrentStrategy(GUIState::Strategy::STRATEGY_DEFAULT);
    setCurrentStrategyRunning(false);
    GUIState::getInstance()->setMouseAreaActive(true);
}

/*!
 * \brief sets abortStrategyIssued to true, causing strategies to stop
 */
void GUIController::abortStrategy()
{
    abortStrategyIssued = true;
}

/*!
 * \brief starts the currently selected strategy
 * \param delay delay between frames in ms
 * \param show how many frames to display before the end of the AutoTracklet
 *
 * The following strategies are currently implemented:
 * - STRATEGY_CLICK_JUMP (delay X, frames Y):
 *     Displays the last Y frames of the AutoTracklet before the end of the
 *     currently selected AutoTracklet with a delay of Y ms inbetween
 * - STRATEGY_CLICK_SPIN (delay X):
 *     Displays all frames to the end of the AutoTracklet with a delay of X ms
 *     inbetween, then starts from the beginning of the AutoTracklet and loops
 *     until abortStrategyIssued is set to true
 * - STRATEGY_CLICK_STEP (delay X):
 *     Displays all frames to the end of the AutoTracklet with a delay of X ms
 *     inbetween and then stops.
 *
 * If no strategy is selected, currentStrategy is set to STRATEGY_DEFAULT.
 */
void GUIController::startStrategy(unsigned long delay, unsigned int show) {
    QFuture<void> f;
    abortStrategyIssued = false;
    GUIState::getInstance()->setMouseAreaActive(false);
    switch (currentStrategy) {
    case GUIState::Strategy::STRATEGY_CLICK_JUMP:
        f = QtConcurrent::run(this, &GUIController::runStrategyClickJump, delay, show);
        break;
    case GUIState::Strategy::STRATEGY_CLICK_SPIN:
        f = QtConcurrent::run(this, &GUIController::runStrategyClickSpin, delay);
        break;
    case GUIState::Strategy::STRATEGY_CLICK_STEP:
        f = QtConcurrent::run(this, &GUIController::runStrategyClickStep, delay);
        break;
    case GUIState::Strategy::STRATEGY_DEFAULT:
        throw CTUnimplementedException("It shouldn't be possible to call startStrategy with STRATEGY_DEFAULT as the current strategy");
    }
    futures.push_back(f);
    setCurrentStrategyRunning(true);
}

/*!
 * \brief calls Genealogy::connectObjects from QML using the selected Object as first and the hovered as second
 * \return the return value of connectObjects
 */
bool GUIController::connectTracks() {
    /* see which cell is under the mouse */
    GUIState *gs = GUIState::getInstance();
    double x = gs->getMouseX();
    double y = gs->getMouseY();
    int frame = gs->getCurrentFrame();
    int slice = gs->getCurrentSlice();
    int channel = gs->getCurrentChannel();

    std::shared_ptr<Object> first = gs->getSelectedCell().lock();
    std::shared_ptr<Object> second = DataProvider::getInstance()->cellAtFrame(frame, slice, channel, x, y);
    if (first && second) {
        return GUIState::getInstance()->getProj()->getGenealogy()->connectObjects(first, second);
    }
    return false;
}

}

