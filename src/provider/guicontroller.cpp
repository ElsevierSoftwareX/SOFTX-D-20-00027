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

void GUIController::changeFrame(int newFrame) {
    GUIState::getInstance()->setCurrentFrame(newFrame);
}

void GUIController::changeStrategy(GUIState::Strategy strat) {

}

void GUIController::changeAction(GUIState::Action act) {
}


}

