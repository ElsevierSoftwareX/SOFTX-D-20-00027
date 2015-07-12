#include "guicontroller.h"
#include "guistate.h"

#include "exceptions/ctunimplementedexception.h"

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

//void ImageProvider2::setMotherCell()
//{
//    if(GUIState::getInstance()->getNewSelectedCell() != nullptr) {
//        GUIState::getInstance()->setStrategyStep(2);
//        GUIState::getInstance()->setMotherCell(GUIState::getInstance()->getNewSelectedCell());
//        GUIState::getInstance()->getDaughterCells().clear();
//        GUIState::getInstance()->setStatus("Select daughter objects - press space when finished");
//    }
//    else {
//        GUIState::getInstance()->setStatus("Select mother track");
//    }
//}

//void ImageProvider2::setDaughterCells()
//{
//    std::shared_ptr<Tracklet> mother = DataProvider::getInstance()->getProj()->getGenealogy()->getTracklet(GUIState::getInstance()->getMotherCell()->getAutoId());
//    for(int i = 0; i < GUIState::getInstance()->getDaughterCells().size(); ++i) {
//        std::shared_ptr<Tracklet> daughter = DataProvider::getInstance()->getProj()->getGenealogy()->getTracklet(GUIState::getInstance()->getDaughterCells().at(i)->getAutoId());
//        DataProvider::getInstance()->getProj()->getGenealogy()->addDaughterTrack(mother, daughter);
//    }
//    GUIState::getInstance()->setStatus("Daughter tracks added");
//    GUIState::getInstance()->getDaughterCells().clear();
//}


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

void GUIController::changeStrategy(int strat) {
    currentStrategy = static_cast<GUIState::Strategy>(strat);
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


}

