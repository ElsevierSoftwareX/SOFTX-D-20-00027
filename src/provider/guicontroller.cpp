#include "guicontroller.h"

#include <QtConcurrent/QtConcurrent>

#include "guistate.h"
#include "exceptions/ctunimplementedexception.h"
#include "tracked/trackevent.h"
#include "tracked/trackeventdivision.h"

namespace CellTracker {

GUIController *GUIController::theInstance = nullptr;

GUIController::GUIController(QObject *parent) :
    QObject(parent),
    abortStrategyIssued(false) {}

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

/*!
 * \brief changes the current Frame to a specific value
 * \param newFrame the number of the new Frame
 */
void GUIController::changeFrameAbs(int newFrame) {
    GUIState::getInstance()->setCurrentFrame(newFrame);
    QObject *s = GUIState::getInstance()->getSlider();
    if (s) s->setProperty("value", newFrame);
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
void GUIController::hoverCell(std::shared_ptr<Object> o) {
    GUIState::getInstance()->setHoveredCell(o);
    GUIState::getInstance()->setHoveredCellID(o->getId());
}

/*!
 * \brief resets the hovered Object
 */
void GUIController::unhoverCell() {
    GUIState::getInstance()->setHoveredCell(nullptr);
    GUIState::getInstance()->setHoveredCellID(-1);
}

/*!
 * \brief sets the Tracklet of an Object as hovered
 * \param o the Object whose Tracklet should be set hovered
 * \param proj the current Project
 */
void GUIController::hoverTrack(std::shared_ptr<Object> o, std::shared_ptr<Project> proj) {
    std::shared_ptr<Tracklet> t = proj->getGenealogy()->getTracklet(o->getTrackId());
    GUIState::getInstance()->setHoveredTrackID(t->getId());
    uint32_t start = t->getStart().first->getID();
    uint32_t end = t->getEnd().first->getID();
    uint32_t length = end - start;
    QString status = t->qmlStatus();
    QString mother = t->qmlMother();
    QString daughters = t->qmlDaughters();

    GUIState::getInstance()->setHoveredTrackStart(start);
    GUIState::getInstance()->setHoveredTrackEnd(end);
    GUIState::getInstance()->setHoveredTrackLength(length);
    GUIState::getInstance()->setHoveredTrackStatus(status);
    GUIState::getInstance()->setHoveredTrackMother(mother);
    GUIState::getInstance()->setHoveredTrackDaughters(daughters);
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
    GUIState::getInstance()->setHoveredTrackMother("");
    GUIState::getInstance()->setHoveredTrackDaughters("");
}

/*!
 * \brief sets the AutoTracklet of an Object as hovered
 * \param o the Object whose AutoTracklet should be set hovered
 * \param proj the current Project
 */
void GUIController::hoverAutoTracklet(std::shared_ptr<Object> o, std::shared_ptr<Project> proj) {
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
 * \param x
 * \param y
 */
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

/*!
 * \brief sets an Object as selected
 * \param o the Object to set selected
 */
void GUIController::selectCell(std::shared_ptr<Object> o) {
    GUIState::getInstance()->setSelectedCell(o);
    GUIState::getInstance()->setSelectedCellID(o->getId());
    GUIState::getInstance()->setSelectedCellFrame(o->getFrameId());
}

/*!
 * \brief resets the selected Object
 */
void GUIController::deselectCell() {
    GUIState::getInstance()->setSelectedCell(nullptr);
    GUIState::getInstance()->setSelectedCellID(-1);
    GUIState::getInstance()->setSelectedCellFrame(-1);
}

/*!
 * \brief sets the Tracklet of an Object as selected
 * \param o the Object whose Tracklet should be set selected
 * \param proj the current Project
 */
void GUIController::selectTrack(std::shared_ptr<Object> o, std::shared_ptr<Project> proj) {
    std::shared_ptr<Tracklet> t = proj->getGenealogy()->getTracklet(o->getTrackId());
    GUIState::getInstance()->setSelectedTrackID(t->getId());
    uint32_t start = t->getStart().first->getID();
    uint32_t end = t->getEnd().first->getID();
    uint32_t length = end - start;
    QString status = t->qmlStatus();
    QString mother = t->qmlMother();
    QString daughters = t->qmlDaughters();

    GUIState::getInstance()->setSelectedTrackStart(start);
    GUIState::getInstance()->setSelectedTrackEnd(end);
    GUIState::getInstance()->setSelectedTrackLength(length);
    GUIState::getInstance()->setSelectedTrackStatus(status);
    GUIState::getInstance()->setSelectedTrackMother(mother);
    GUIState::getInstance()->setSelectedTrackDaughters(daughters);
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
    GUIState::getInstance()->setSelectedTrackMother("");
    GUIState::getInstance()->setSelectedTrackDaughters("");
}

/*!
 * \brief sets the AutoTracklet of an Object as selected
 * \param o the Object whose AutoTracklet should be set selected
 * \param proj the current Project
 */
void GUIController::selectAutoTracklet(std::shared_ptr<Object> o, std::shared_ptr<Project> proj) {
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
 * \param x
 * \param y
 *
 * Depending on the currently selected Action, this might perform those Actions
 */
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

        if (mother->getFrameId() > daughter->getFrameId())
            return;

        std::shared_ptr<Tracklet> motherT;
        motherT = proj->getGenealogy()->getTracklet(mother->getTrackId());

        if (!motherT)
            return;

        proj->getGenealogy()->addDaughterTrack(motherT, daughter);
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
            if (currentFrame >= 0 && val.first->getID() >= (uint32_t)currentFrame)
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
            if (currentFrame >= 0 && val.first->getID() <= (uint32_t)currentFrame)
                t->removeFromContained(val.first->getID(), val.second->getId());
        }

        if (t->getContained().isEmpty()) /* remove tracklet if there are no more cells in it */
            proj->getGenealogy()->removeTracklet(t->getId());

        GUIState::getInstance()->backingDataChanged();
        break;
    }
    }
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
    std::shared_ptr<AutoTracklet> t = GUIState::getInstance()->getSelectedAutoTrack();

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
        GUIController::getInstance()->changeFrame(1);
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
    std::shared_ptr<AutoTracklet> t = GUIState::getInstance()->getSelectedAutoTrack();

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
        GUIController::getInstance()->changeFrameAbs(curr);
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
    std::shared_ptr<AutoTracklet> t = GUIState::getInstance()->getSelectedAutoTrack();

    if (!t)
        goto out;

    {
    uint32_t end = t->getEnd();

    unsigned int curr = GUIState::getInstance()->getCurrentFrame();

    if (curr >= end) /* nothing to do, we are at the end of or after the track */
        goto out;

    for (unsigned int curr=GUIState::getInstance()->getCurrentFrame(); curr <= end; curr++) {
        if (abortStrategyIssued)
            break;
        QThread::msleep(delay);
        GUIController::getInstance()->changeFrameAbs(curr);
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
    abortStrategyIssued = false;
    GUIState::getInstance()->setMouseAreaActive(false);
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
    case GUIState::Strategy::STRATEGY_DEFAULT:
        throw CTUnimplementedException("It shouldn't be possible to call startStrategy with STRATEGY_DEFAULT as the current strategy");
    }
    setCurrentStrategyRunning(true);
}

/*!
 * \brief calls Genealogy::connectObjects from QML using the selected Object as first and the hovered as second
 * \return the return value of connectObjects
 */
bool GUIController::connectTracks() {
    /* see which cell is under the mouse */
    GUIState *gs = GUIState::getInstance();
    float x = gs->getMouseX();
    float y = gs->getMouseY();
    int frame = gs->getCurrentFrame();

    std::shared_ptr<Object> first = gs->getSelectedCell();
    std::shared_ptr<Object> second = DataProvider::getInstance()->cellAtFrame(frame, x, y);
    if (first && second) {
        return GUIState::getInstance()->getProj()->getGenealogy()->connectObjects(first, second);
    }
    return false;
}

}

