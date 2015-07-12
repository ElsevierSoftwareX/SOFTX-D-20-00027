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

void GUIController::selectCell(int frame, int x, int y){
    std::shared_ptr<Project> proj = GUIState::getInstance()->getNewProj();
    std::shared_ptr<Object> o = DataProvider::getInstance()->cellAtFrame(frame, x, y);

    if (!proj || !o) /* either we don't have a project yet or there simply is no cell at this position */
        return;

    switch (currentAction) {
    case GUIState::Action::ACTION_DEFAULT:
    {

        GUIState::getInstance()->setNewSelectedCell(o);
        GUIState::getInstance()->setNewSelectedCellID(o->getId());

        if (o->isInTracklet()) {
            std::shared_ptr<Tracklet> t = proj->getGenealogy()->getTracklet(o->getTrackId());
            GUIState::getInstance()->setNewSelectedTrackID(t->getID());
            uint32_t start = t->getStart().first->getID();
            uint32_t end = t->getEnd().first->getID();
            uint32_t length = end - start;

            GUIState::getInstance()->setNewSelectedTrackStart(start);
            GUIState::getInstance()->setNewSelectedTrackEnd(end);
            GUIState::getInstance()->setNewSelectedTrackLength(length);
        } else {
            GUIState::getInstance()->setNewSelectedTrackID(-1);
            GUIState::getInstance()->setNewSelectedTrackStart(-1);
            GUIState::getInstance()->setNewSelectedTrackEnd(-1);
            GUIState::getInstance()->setNewSelectedTrackLength(-1);
        }

        if (o->isInAutoTracklet()) {
            std::shared_ptr<AutoTracklet> at = proj->getAutoTracklet(o->getAutoId());
            GUIState::getInstance()->setNewSelectedAutoTrackID(at->getID());
            uint32_t start = at->getStart().first->getID();
            uint32_t end = at->getEnd().first->getID();
            uint32_t length = at->getLength();

            GUIState::getInstance()->setNewSelectedAutoTrackStart(start);
            GUIState::getInstance()->setNewSelectedAutoTrackEnd(end);
            GUIState::getInstance()->setNewSelectedAutoTrackLength(length);
        } else {
            GUIState::getInstance()->setNewSelectedAutoTrackID(-1);
            GUIState::getInstance()->setNewSelectedAutoTrackStart(-1);
            GUIState::getInstance()->setNewSelectedAutoTrackEnd(-1);
            GUIState::getInstance()->setNewSelectedAutoTrackLength(-1);
        }
        break;
    }
    case GUIState::Action::ACTION_ADD_DAUGHTERS:
    {
        std::shared_ptr<Object> mother, daughter;
        mother = GUIState::getInstance()->getNewSelectedCell();
        daughter = o;

        std::shared_ptr<Tracklet> motherT, daughterT;
        motherT = proj->getGenealogy()->getTracklet(mother->getTrackId());

        proj->getGenealogy()->addDaughterTrack(motherT, daughter);
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

