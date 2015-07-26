#include "guicontroller.h"
#include "guistate.h"

#include "exceptions/ctunimplementedexception.h"
#include "corrected_data/trackevent.h"
#include "corrected_data/trackeventdivision.h"

namespace CellTracker {

void WorkerThread::run() {
    /* example */
    for (int i = 0; i < 100; i++) {
        GUIController::getInstance()->changeFrame(1);
        this->msleep(1000/24);
    }
    qDebug() << "bla in thread";
}

GUIController *GUIController::theInstance = nullptr;

GUIController::GUIController(QObject *parent) : QObject(parent){}

GUIController *GUIController::getInstance() {
    if (!theInstance)
        theInstance = new GUIController();
    return theInstance;
}

QObject *GUIController::qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return getInstance();
}

void GUIController::changeFrameAbs(int newFrame) {
    GUIState::getInstance()->setNewCurrentFrame(newFrame);
}

void GUIController::changeFrame(int diff) {
    int curr = GUIState::getInstance()->getNewCurrentFrame();
    if (curr + diff < 0)
        GUIState::getInstance()->setNewCurrentFrame(0);
    else if (curr + diff > GUIState::getInstance()->getNewMaximumFrame())
        GUIState::getInstance()->setNewCurrentFrame(GUIState::getInstance()->getNewMaximumFrame());
    else
        GUIState::getInstance()->setNewCurrentFrame(curr+diff);
}

void GUIController::changeStrategy(int strat) {
    currentStrategy = static_cast<GUIState::Strategy>(strat);
    emit currentStrategyChanged(currentStrategy);
}

void GUIController::changeAction(int act) {
    currentAction = static_cast<GUIState::Action>(act);
    emit currentActionChanged(currentAction);
}

void GUIController::hoverCell(std::shared_ptr<Object> o) {
    GUIState::getInstance()->setNewHoveredCell(o);
    GUIState::getInstance()->setNewHoveredCellID(o->getId());
}

void GUIController::unhoverCell() {
    GUIState::getInstance()->setNewHoveredCell(nullptr);
    GUIState::getInstance()->setNewHoveredCellID(-1);
}

void GUIController::hoverTrack(std::shared_ptr<Object> o, std::shared_ptr<Project> proj) {
    std::shared_ptr<Tracklet> t = proj->getGenealogy()->getTracklet(o->getTrackId());
    GUIState::getInstance()->setNewHoveredTrackID(t->getID());
    uint32_t start = t->getStart().first->getID();
    uint32_t end = t->getEnd().first->getID();
    uint32_t length = end - start;

    GUIState::getInstance()->setNewHoveredTrackStart(start);
    GUIState::getInstance()->setNewHoveredTrackEnd(end);
    GUIState::getInstance()->setNewHoveredTrackLength(length);
}

void GUIController::unhoverTrack() {
    GUIState::getInstance()->setNewHoveredTrackID(-1);
    GUIState::getInstance()->setNewHoveredTrackStart(-1);
    GUIState::getInstance()->setNewHoveredTrackEnd(-1);
    GUIState::getInstance()->setNewHoveredTrackLength(-1);
}

void GUIController::hoverAutoTracklet(std::shared_ptr<Object> o, std::shared_ptr<Project> proj) {
    std::shared_ptr<AutoTracklet> at = proj->getAutoTracklet(o->getAutoId());
    GUIState::getInstance()->setNewHoveredAutoTrackID(at->getID());
    uint32_t start = at->getStart().first->getID();
    uint32_t end = at->getEnd().first->getID();
    uint32_t length = at->getLength();

    GUIState::getInstance()->setNewHoveredAutoTrackStart(start);
    GUIState::getInstance()->setNewHoveredAutoTrackEnd(end);
    GUIState::getInstance()->setNewHoveredAutoTrackLength(length);
}

void GUIController::unhoverAutoTracklet() {
    GUIState::getInstance()->setNewHoveredAutoTrackID(-1);
    GUIState::getInstance()->setNewHoveredAutoTrackStart(-1);
    GUIState::getInstance()->setNewHoveredAutoTrackEnd(-1);
    GUIState::getInstance()->setNewHoveredAutoTrackLength(-1);
}

void GUIController::hoverCell(int frame, int x, int y){
    std::shared_ptr<Project> proj = GUIState::getInstance()->getNewProj();
    std::shared_ptr<Object> o = DataProvider::getInstance()->cellAtFrame(frame, x, y);

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

void GUIController::selectCell(std::shared_ptr<Object> o) {
    GUIState::getInstance()->setNewSelectedCell(o);
    GUIState::getInstance()->setNewSelectedCellID(o->getId());
}

void GUIController::deselectCell() {
    GUIState::getInstance()->setNewSelectedCell(nullptr);
    GUIState::getInstance()->setNewSelectedCellID(-1);
}

void GUIController::selectTrack(std::shared_ptr<Object> o, std::shared_ptr<Project> proj) {
    std::shared_ptr<Tracklet> t = proj->getGenealogy()->getTracklet(o->getTrackId());
    GUIState::getInstance()->setNewSelectedTrackID(t->getID());
    uint32_t start = t->getStart().first->getID();
    uint32_t end = t->getEnd().first->getID();
    uint32_t length = end - start;

    GUIState::getInstance()->setNewSelectedTrackStart(start);
    GUIState::getInstance()->setNewSelectedTrackEnd(end);
    GUIState::getInstance()->setNewSelectedTrackLength(length);
}

void GUIController::deselectTrack() {
    GUIState::getInstance()->setNewSelectedTrackID(-1);
    GUIState::getInstance()->setNewSelectedTrackStart(-1);
    GUIState::getInstance()->setNewSelectedTrackEnd(-1);
    GUIState::getInstance()->setNewSelectedTrackLength(-1);
}

void GUIController::selectAutoTracklet(std::shared_ptr<Object> o, std::shared_ptr<Project> proj) {
    std::shared_ptr<AutoTracklet> at = proj->getAutoTracklet(o->getAutoId());
    GUIState::getInstance()->setNewSelectedAutoTrackID(at->getID());
    uint32_t start = at->getStart().first->getID();
    uint32_t end = at->getEnd().first->getID();
    uint32_t length = at->getLength();

    GUIState::getInstance()->setNewSelectedAutoTrackStart(start);
    GUIState::getInstance()->setNewSelectedAutoTrackEnd(end);
    GUIState::getInstance()->setNewSelectedAutoTrackLength(length);
}

void GUIController::deselectAutoTracklet() {
    GUIState::getInstance()->setNewSelectedAutoTrackID(-1);
    GUIState::getInstance()->setNewSelectedAutoTrackStart(-1);
    GUIState::getInstance()->setNewSelectedAutoTrackEnd(-1);
    GUIState::getInstance()->setNewSelectedAutoTrackLength(-1);
}

void GUIController::selectCell(int frame, int x, int y){
    std::shared_ptr<Project> proj = GUIState::getInstance()->getNewProj();
    std::shared_ptr<Object> o = DataProvider::getInstance()->cellAtFrame(frame, x, y);

    if (!proj) /* we don't have a project yet */
        return;

    if (!o && currentAction == GUIState::Action::ACTION_DEFAULT) { /* only do this when action is default */
        deselectCell();
        deselectTrack();
        deselectAutoTracklet();
        return;
    }

    switch (currentAction) {
    case GUIState::Action::ACTION_DEFAULT:
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
    case GUIState::Action::ACTION_ADD_DAUGHTERS:
    {
        std::shared_ptr<Object> mother, daughter;
        mother = GUIState::getInstance()->getNewSelectedCell();
        daughter = o;

        if (!mother || !daughter)
            return;

        std::shared_ptr<Tracklet> motherT;
        motherT = proj->getGenealogy()->getTracklet(mother->getTrackId());

        if (!motherT)
            return;

        proj->getGenealogy()->addDaughterTrack(motherT, daughter);
        break;
    }
    case GUIState::ACTION_DELETE_CELL:
    {
        std::shared_ptr<Object> cell;
        int currentFrame;

        cell = o;
        currentFrame = GUIState::getInstance()->getNewCurrentFrame();

        if (!cell) /* no cell selected */
            return;

        std::shared_ptr<Tracklet> t = proj->getGenealogy()->getTracklet(cell->getTrackId());

        if (!t)
            return;

        t->removeFromContained(currentFrame, cell->getId());

        if (t->getContained().isEmpty()) /* remove tracklet if there are no more cells in it */
            proj->getGenealogy()->removeTracklet(t->getID());
        break;
    }
    default:
        break;
    }
}

int GUIController::getCurrentStrategy() const {
    return static_cast<int>(currentStrategy);
}

int GUIController::getCurrentAction() const {
    return static_cast<int>(currentAction);
}

void GUIController::startStrategy() {
    switch (currentStrategy) {
    case GUIState::Strategy::STRATEGY_CLICK_JUMP:
    case GUIState::Strategy::STRATEGY_CLICK_SPIN:
    case GUIState::Strategy::STRATEGY_CLICK_STEP:
    case GUIState::Strategy::STRATEGY_HOVER_STEP:
    default:
        throw CTUnimplementedException("Unimplemented case in startStrategy");
        break;
    }
}

void GUIController::connectTracks() {
    /* see which cell is under the mouse */
    GUIState *gs = GUIState::getInstance();
    float x = gs->getNewMouseX();
    float y = gs->getNewMouseY();
    int frame = gs->getNewCurrentFrame();

    std::shared_ptr<Object> first = gs->getNewSelectedCell();
    std::shared_ptr<Object> second = DataProvider::getInstance()->cellAtFrame(frame, x, y);
    if (first && second) {
        /*! \todo check return value */
        GUIState::getInstance()->getNewProj()->getGenealogy()->connectObjects(first, second);
    }
}


}

