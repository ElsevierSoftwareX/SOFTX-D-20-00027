#include "guistate.h"

#include <QDebug>

namespace CellTracker {

GUIState *GUIState::theInstance = nullptr;

GUIState *GUIState::getInstance(){
    if (!theInstance)
        theInstance = new GUIState();
    return theInstance;
}

GUIState::GUIState(QObject *parent) : QObject(parent),
    newProj(nullptr),
    newProjPath(""),
    newCurrentFrame(0),
    newMaximumFrame(0),
    newMouseX(0),
    newMouseY(0),
    newMouseAreaActive(true),
    newSelectedCell(nullptr),
    newSelectedCellID(-1),
    newSelectedAutoTrack(nullptr),
    newSelectedAutoTrackStart(-1),
    newSelectedAutoTrackEnd(-1),
    newSelectedAutoTrackLength(-1),
    newSelectedTrack(nullptr),
    newSelectedTrackStart(-1),
    newSelectedTrackEnd(-1),
    newSelectedTrackLength(-1),
    newHoveredCell(nullptr),
    newHoveredCellID(-1),
    newHoveredAutoTrack(nullptr),
    newHoveredAutoTrackStart(-1),
    newHoveredAutoTrackEnd(-1),
    newHoveredAutoTrackLength(-1),
    newHoveredTrack(nullptr),
    newHoveredTrackStart(-1),
    newHoveredTrackEnd(-1),
    newHoveredTrackLength(-1),
    newSelectedAutoTrackID(-1),
    newSelectedTrackID(-1),
    newHoveredAutoTrackID(-1),
    newHoveredTrackID(-1)
{}

QObject *GUIState::qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return getInstance();
}

}
