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
    proj(nullptr),
    projPath(""),
    currentFrame(0),
    maximumFrame(0),
    mouseX(0),
    mouseY(0),
    mouseAreaActive(true),
    selectedCell(nullptr),
    selectedCellID(-1),
    selectedCellFrame(-1),
    selectedAutoTrack(nullptr),
    selectedAutoTrackStart(-1),
    selectedAutoTrackEnd(-1),
    selectedAutoTrackLength(-1),
    selectedTrack(nullptr),
    selectedTrackStart(-1),
    selectedTrackEnd(-1),
    selectedTrackLength(-1),
    selectedTrackStatus(""),
    hoveredCell(nullptr),
    hoveredCellID(-1),
    hoveredCellFrame(-1),
    hoveredAutoTrack(nullptr),
    hoveredAutoTrackStart(-1),
    hoveredAutoTrackEnd(-1),
    hoveredAutoTrackLength(-1),
    hoveredTrack(nullptr),
    hoveredTrackStart(-1),
    hoveredTrackEnd(-1),
    hoveredTrackLength(-1),
    hoveredTrackStatus(""),
    drawTrackletIDs(true),
    drawAnnotationInfo(true),
    drawOutlines(true),
    drawCutLine(false),
    startX(-1),
    startY(-1),
    zoomFactor(1.0),
    offX(0),
    offY(0),
    selectedAutoTrackID(-1),
    selectedTrackID(-1),
    hoveredAutoTrackID(-1),
    hoveredTrackID(-1) {}

QObject *GUIState::qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return getInstance();
}

}
