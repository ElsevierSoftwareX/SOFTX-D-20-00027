#include "guicontroller.h"
#include "guistate.h"

namespace CellTracker {

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
    else if (curr + diff > GUIState::getInstance()->getMaximumValue())
        GUIState::getInstance()->setNewCurrentFrame(GUIState::getInstance()->getMaximumValue());
    else
        GUIState::getInstance()->setNewCurrentFrame(curr+diff);
}

void GUIController::changeStrategy(GUIState::Strategy strat) {

}

void GUIController::changeAction(GUIState::Action act) {
}

void GUIController::selectCell(int frame, int x, int y){
    std::shared_ptr<Project> proj = DataProvider::getInstance()->getProj();
    std::shared_ptr<Object> o = DataProvider::getInstance()->cellAtFrame(frame, x, y);
    if (!proj || !o) /* either we don't have a project yet or there simply is no cell at this position */
        return;

    std::shared_ptr<AutoTracklet> t = proj->getAutoTracklet(o->getAutoId());

    GUIState::getInstance()->setNewSelectedCell(o);
    GUIState::getInstance()->setNewSelectedCellID(o->getId());
    GUIState::getInstance()->setNewSelectedTrackID(t->getID());

    uint32_t start = t->getStart().first->getID();
    uint32_t end = t->getEnd().first->getID();
    uint32_t length = t->getLength();

    GUIState::getInstance()->setNewSelectedTrackStart(start);
    GUIState::getInstance()->setNewSelectedTrackEnd(end);
    GUIState::getInstance()->setNewSelectedTrackLength(length);
}

}

