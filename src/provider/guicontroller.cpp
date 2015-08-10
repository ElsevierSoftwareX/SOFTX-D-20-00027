#include "guicontroller.h"

#include <QtConcurrent/QtConcurrent>

#include "guistate.h"
#include "exceptions/ctunimplementedexception.h"
#include "corrected_data/trackevent.h"
#include "corrected_data/trackeventdivision.h"

namespace CellTracker {

GUIController *GUIController::theInstance = nullptr;

GUIController::GUIController(QObject *parent) : QObject(parent), abortStrategyIssued(false)
{}

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
    GUIState::getInstance()->setCurrentFrame(newFrame);
    QObject *s = GUIState::getInstance()->getSlider();
    s->setProperty("value", newFrame);
}

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
    s->setProperty("value", nVal);
}

void GUIController::hoverCell(std::shared_ptr<Object> o) {
    GUIState::getInstance()->setHoveredCell(o);
    GUIState::getInstance()->setHoveredCellID(o->getId());
}

void GUIController::unhoverCell() {
    GUIState::getInstance()->setHoveredCell(nullptr);
    GUIState::getInstance()->setHoveredCellID(-1);
}

void GUIController::hoverTrack(std::shared_ptr<Object> o, std::shared_ptr<Project> proj) {
    std::shared_ptr<Tracklet> t = proj->getGenealogy()->getTracklet(o->getTrackId());
    GUIState::getInstance()->setHoveredTrackID(t->getID());
    uint32_t start = t->getStart().first->getID();
    uint32_t end = t->getEnd().first->getID();
    uint32_t length = end - start;

    GUIState::getInstance()->setHoveredTrackStart(start);
    GUIState::getInstance()->setHoveredTrackEnd(end);
    GUIState::getInstance()->setHoveredTrackLength(length);
}

void GUIController::unhoverTrack() {
    GUIState::getInstance()->setHoveredTrackID(-1);
    GUIState::getInstance()->setHoveredTrackStart(-1);
    GUIState::getInstance()->setHoveredTrackEnd(-1);
    GUIState::getInstance()->setHoveredTrackLength(-1);
}

void GUIController::hoverAutoTracklet(std::shared_ptr<Object> o, std::shared_ptr<Project> proj) {
    std::shared_ptr<AutoTracklet> at = proj->getAutoTracklet(o->getAutoId());
    GUIState::getInstance()->setHoveredAutoTrackID(at->getID());
    uint32_t start = at->getStart();
    uint32_t end = at->getEnd();
//    uint32_t start = at->getStart().first->getID();
//    uint32_t end = at->getEnd().first->getID();
    uint32_t length = at->getLength();

    GUIState::getInstance()->setHoveredAutoTrackStart(start);
    GUIState::getInstance()->setHoveredAutoTrackEnd(end);
    GUIState::getInstance()->setHoveredAutoTrackLength(length);
}

void GUIController::unhoverAutoTracklet() {
    GUIState::getInstance()->setHoveredAutoTrackID(-1);
    GUIState::getInstance()->setHoveredAutoTrackStart(-1);
    GUIState::getInstance()->setHoveredAutoTrackEnd(-1);
    GUIState::getInstance()->setHoveredAutoTrackLength(-1);
}

void GUIController::hoverCell(int frame, int x, int y){
    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
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
    GUIState::getInstance()->setSelectedCell(o);
    GUIState::getInstance()->setSelectedCellID(o->getId());
}

void GUIController::deselectCell() {
    GUIState::getInstance()->setSelectedCell(nullptr);
    GUIState::getInstance()->setSelectedCellID(-1);
}

void GUIController::selectTrack(std::shared_ptr<Object> o, std::shared_ptr<Project> proj) {
    std::shared_ptr<Tracklet> t = proj->getGenealogy()->getTracklet(o->getTrackId());
    GUIState::getInstance()->setSelectedTrackID(t->getID());
    uint32_t start = t->getStart().first->getID();
    uint32_t end = t->getEnd().first->getID();
    uint32_t length = end - start;

    GUIState::getInstance()->setSelectedTrackStart(start);
    GUIState::getInstance()->setSelectedTrackEnd(end);
    GUIState::getInstance()->setSelectedTrackLength(length);
}

void GUIController::deselectTrack() {
    GUIState::getInstance()->setSelectedTrackID(-1);
    GUIState::getInstance()->setSelectedTrackStart(-1);
    GUIState::getInstance()->setSelectedTrackEnd(-1);
    GUIState::getInstance()->setSelectedTrackLength(-1);
}

void GUIController::selectAutoTracklet(std::shared_ptr<Object> o, std::shared_ptr<Project> proj) {
    std::shared_ptr<AutoTracklet> at = proj->getAutoTracklet(o->getAutoId());
    GUIState::getInstance()->setSelectedAutoTrackID(at->getID());
    uint32_t start = at->getStart();
    uint32_t end = at->getEnd();
//    uint32_t start = at->getStart().first->getID();
//    uint32_t end = at->getEnd().first->getID();
    uint32_t length = at->getLength();

    GUIState::getInstance()->setSelectedAutoTrackStart(start);
    GUIState::getInstance()->setSelectedAutoTrackEnd(end);
    GUIState::getInstance()->setSelectedAutoTrackLength(length);
}

void GUIController::deselectAutoTracklet() {
    GUIState::getInstance()->setSelectedAutoTrackID(-1);
    GUIState::getInstance()->setSelectedAutoTrackStart(-1);
    GUIState::getInstance()->setSelectedAutoTrackEnd(-1);
    GUIState::getInstance()->setSelectedAutoTrackLength(-1);
}

void GUIController::selectCell(int frame, int x, int y){
    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
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
        mother = GUIState::getInstance()->getSelectedCell();
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
        currentFrame = GUIState::getInstance()->getCurrentFrame();

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

bool GUIController::getCurrentStrategyRunning() const
{
    return currentStrategyRunning;
}

int GUIController::getCurrentAction() const {
    return static_cast<int>(currentAction);
}

void GUIController::setCurrentStrategy(int value)
{
    GUIState::Strategy newVal = static_cast<GUIState::Strategy>(value);
    if (currentStrategy != newVal)
        emit currentStrategyChanged(currentStrategy = newVal);
}

void GUIController::setCurrentStrategyRunning(bool value)
{
    if (currentStrategyRunning != value)
        emit currentStrategyRunningChanged(currentStrategyRunning = value);
}

void GUIController::setCurrentAction(int value)
{
    GUIState::Action newVal = static_cast<GUIState::Action>(value);
    if (currentAction != newVal)
        emit currentActionChanged(currentAction = newVal);
}

void GUIController::runStrategyClickJump(unsigned long delay, unsigned int show) {
    int jumpFrames;
    int displayFrames = show;

    /* get current track */
    std::shared_ptr<AutoTracklet> t = GUIState::getInstance()->getSelectedAutoTrack();

    /* get length of current track */
    uint32_t start = t->getStart();
    uint32_t end = t->getEnd();
//    uint32_t start = t->getStart().first->getID();
//    uint32_t end = t->getEnd().first->getID();

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
        GUIController::getInstance()->changeFrame(1);
    }
out:
    abortStrategyIssued = false;
    setCurrentStrategy(GUIState::Strategy::STRATEGY_DEFAULT);
    setCurrentStrategyRunning(false);
}

void GUIController::runStrategyClickSpin(unsigned long delay) {
    /* get current track */
    std::shared_ptr<AutoTracklet> t = GUIState::getInstance()->getSelectedAutoTrack();

    /* get length of current track */
    uint32_t start = t->getStart();
    uint32_t end = t->getEnd();
//    uint32_t start = t->getStart().first->getID();
//    uint32_t end = t->getEnd().first->getID();

    uint32_t begin = GUIState::getInstance()->getCurrentFrame();
    unsigned int curr = begin;

    if (begin < start || begin >= end) /* nothing to do, we are at the end of or after the track */
        goto out;

    while (true) {
        if (abortStrategyIssued)
            break;
        curr = (curr >= end)?begin:curr+1;
        QThread::msleep(delay);
        GUIController::getInstance()->changeFrameAbs(curr);
    }
out:
    abortStrategyIssued = false;
    setCurrentStrategy(GUIState::Strategy::STRATEGY_DEFAULT);
    setCurrentStrategyRunning(false);
}

void GUIController::runStrategyClickStep(unsigned long delay) {
    /* get current track */
    std::shared_ptr<AutoTracklet> t = GUIState::getInstance()->getSelectedAutoTrack();

    uint32_t end = t->getEnd();
//    uint32_t end = t->getEnd().first->getID();

    unsigned int curr = GUIState::getInstance()->getCurrentFrame();

    if (curr >= end) /* nothing to do, we are at the end of or after the track */
        goto out;

    for (unsigned int curr=GUIState::getInstance()->getCurrentFrame(); curr <= end; curr++) {
        if (abortStrategyIssued)
            break;
        QThread::msleep(delay);
        GUIController::getInstance()->changeFrameAbs(curr);
    }
out:
    abortStrategyIssued = false;
    setCurrentStrategy(GUIState::Strategy::STRATEGY_DEFAULT);
    setCurrentStrategyRunning(false);
}

void GUIController::runStrategyHoverStep(unsigned long delay) {
    Q_UNUSED(delay)
    throw CTUnimplementedException("");
}

void GUIController::abortStrategy()
{
    abortStrategyIssued = true;
}

void GUIController::startStrategy(unsigned long delay, unsigned int show) {
    abortStrategyIssued = false;
    switch (currentStrategy) {
    case GUIState::Strategy::STRATEGY_CLICK_JUMP:
        QtConcurrent::run(this, &GUIController::runStrategyClickJump, delay, show);
        break;
    case GUIState::Strategy::STRATEGY_CLICK_SPIN:
        QtConcurrent::run(this, &GUIController::runStrategyClickSpin, delay);
        break;
    case GUIState::Strategy::STRATEGY_CLICK_STEP:
        QtConcurrent::run(this, &GUIController::runStrategyClickStep, delay);
        break;
    case GUIState::Strategy::STRATEGY_HOVER_STEP:
        QtConcurrent::run(this, &GUIController::runStrategyHoverStep, delay);
    default:
        throw CTUnimplementedException("Unimplemented case in startStrategy");
        break;
    }
    setCurrentStrategyRunning(true);
}

void GUIController::connectTracks() {
    /* see which cell is under the mouse */
    GUIState *gs = GUIState::getInstance();
    float x = gs->getMouseX();
    float y = gs->getMouseY();
    int frame = gs->getCurrentFrame();

    std::shared_ptr<Object> first = gs->getSelectedCell();
    std::shared_ptr<Object> second = DataProvider::getInstance()->cellAtFrame(frame, x, y);
    if (first && second) {
        /*! \todo check return value */
        GUIState::getInstance()->getProj()->getGenealogy()->connectObjects(first, second);
    }
}

}

