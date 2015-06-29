#include "guistate.h"

#include <QDebug>

namespace CellTracker {

CellTracker::GUIState *CellTracker::GUIState::theInstance = nullptr;

CellTracker::GUIState *CellTracker::GUIState::getInstance(){
    if (!theInstance)
        theInstance = new GUIState();
    return theInstance;
}

CellTracker::GUIState::GUIState(QObject *parent) :
    QObject(parent), proj(nullptr), lastObject(nullptr), currentCell(nullptr),
    selectedCell(nullptr), motherCell(nullptr), selectedCellID(-1),
    /* form imageProvider */
    objectID(-1), trackID(-1), currentFrame(0), strategyStep(1),
    selectedTrackID(-1), isInTracklet(false),
    /* from mousePosition */
    maximumValue(1), lastX(0), lastY(0), sliderValue(1), strategy(""),
    jumpStrategy(""), mouseAction(""), mouseAreaActive(true), status(""), path("") {}

QObject *GUIState::qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return getInstance();
}

}
