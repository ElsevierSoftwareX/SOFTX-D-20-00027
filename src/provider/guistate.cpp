#include "guistate.h"

#include <QDebug>

namespace CellTracker {

CellTracker::GUIState *CellTracker::GUIState::theInstance = nullptr;

CellTracker::GUIState *CellTracker::GUIState::getInstance(){
    if (!theInstance)
        theInstance = new GUIState();
    return theInstance;
}

CellTracker::GUIState::GUIState(QObject *parent) : QObject(parent){}

QObject *GUIState::qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return getInstance();
}

}
