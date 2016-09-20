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
    selectedCell(std::weak_ptr<Object>()),
    selectedCellID(-1),
    selectedCellFrame(-1),
    selectedAutoTrack(std::weak_ptr<AutoTracklet>()),
    selectedAutoTrackStart(-1),
    selectedAutoTrackEnd(-1),
    selectedAutoTrackLength(-1),
    selectedTrack(std::weak_ptr<Tracklet>()),
    selectedTrackStart(-1),
    selectedTrackEnd(-1),
    selectedTrackLength(-1),
    selectedTrackStatus(""),
    hoveredCell(std::weak_ptr<Object>()),
    hoveredCellID(-1),
    hoveredCellFrame(-1),
    hoveredAutoTrack(std::weak_ptr<AutoTracklet>()),
    hoveredAutoTrackStart(-1),
    hoveredAutoTrackEnd(-1),
    hoveredAutoTrackLength(-1),
    hoveredTrack(std::weak_ptr<Tracklet>()),
    hoveredTrackStart(-1),
    hoveredTrackEnd(-1),
    hoveredTrackLength(-1),
    hoveredTrackStatus(""),
    drawTrackletIDs(true),
    drawAnnotationInfo(true),
    drawOutlines(true),
    drawCutLine(false),
    drawSeparation(false),
    drawAggregation(false),
    drawDeletion(false),
    startX(-1),
    startY(-1),
    zoomFactor(1.0),
    offX(0),
    offY(0),
    imageReady(false),
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
