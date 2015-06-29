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

bool GUIState::getMouseAreaActive() const
{
    return mouseAreaActive;
}

void GUIState::setMouseAreaActive(bool value)
{
    mouseAreaActive = value;
    emit mouseAreaActiveChanged(value);
}

QString GUIState::getMouseAction() const
{
    return mouseAction;
}

void GUIState::setMouseAction(const QString &value)
{
    mouseAction = value;
    emit mouseActionChanged(value);
}

QString GUIState::getJumpStrategy() const
{
    return jumpStrategy;
}

void GUIState::setJumpStrategy(const QString &value)
{
    jumpStrategy = value;
    emit jumpStrategyChanged(value);
}

QString GUIState::getStrategy() const
{
    return strategy;
}

void GUIState::setStrategy(const QString &value)
{
    strategy = value;
    emit strategyChanged(value);
}

float GUIState::getSliderValue() const
{
    return sliderValue;
}

void GUIState::setSliderValue(float value)
{
    sliderValue = value;
    emit sliderValueChanged(value);
}

float GUIState::getLastY() const
{
    return lastY;
}

void GUIState::setLastY(float value)
{
    lastY = value;
    emit lastYChanged(value);
}

float GUIState::getLastX() const
{
    return lastX;
}

void GUIState::setLastX(float value)
{
    lastX = value;
    emit lastXChanged(value);
}

int GUIState::getMaximumValue() const
{
    return maximumValue;
}

void GUIState::setMaximumValue(int value)
{
    maximumValue = value;
    emit maximumValueChanged(value);
}


int GUIState::getCurrentFrame() const
{
    return currentFrame;
}

void GUIState::setCurrentFrame(int value)
{
    currentFrame = value;
}

QList<std::shared_ptr<CellTracker::Object> > GUIState::getListOfPolygons() const
{
    return listOfPolygons;
}

void GUIState::setListOfPolygons(const QList<std::shared_ptr<CellTracker::Object> > &value)
{
    listOfPolygons = value;
}

QList<std::shared_ptr<CellTracker::Object> > GUIState::getDaughterCells() const
{
    return daughterCells;
}

void GUIState::setDaughterCells(const QList<std::shared_ptr<CellTracker::Object> > &value)
{
    daughterCells = value;
}

std::shared_ptr<CellTracker::Object> GUIState::getMotherCell() const
{
    return motherCell;
}

void GUIState::setMotherCell(const std::shared_ptr<CellTracker::Object> &value)
{
    motherCell = value;
}

std::shared_ptr<CellTracker::Object> GUIState::getSelectedCell() const
{
    return selectedCell;
}

void GUIState::setSelectedCell(const std::shared_ptr<CellTracker::Object> &value)
{
    selectedCell = value;
}

std::shared_ptr<CellTracker::Object> GUIState::getCurrentCell() const
{
    return currentCell;
}

void GUIState::setCurrentCell(const std::shared_ptr<CellTracker::Object> &value)
{
    currentCell = value;
}

std::shared_ptr<CellTracker::Object> GUIState::getLastObject() const
{
    return lastObject;
}

void GUIState::setLastObject(const std::shared_ptr<CellTracker::Object> &value)
{
    lastObject = value;
}

std::shared_ptr<CellTracker::Project> GUIState::getProj() const
{
    return proj;
}

void GUIState::setProj(const std::shared_ptr<CellTracker::Project> &value)
{
    proj = value;
}

QString GUIState::getPath() const
{
    return path;
}

void GUIState::setPath(const QString &value)
{
    path = value;
    emit pathChanged(value);
}

QString GUIState::getStatus() const
{
    return status;
}

void GUIState::setStatus(const QString &value)
{
    status = value;
    emit statusChanged(value);
}

QImage GUIState::getNewImage() const
{
    qDebug() << "Dangerous!";
    return newImage;
}

void GUIState::setNewImage(const QImage &value)
{
    newImage = value;
}

QObject *GUIState::getMouseArea() const
{
    return mouseArea;
}

void GUIState::setMouseArea(QObject *value)
{
    mouseArea = value;
}

uint32_t GUIState::getSelectedCellID() const
{
    return selectedCellID;
}

void GUIState::setSelectedCellID(const uint32_t &value)
{
    selectedCellID = value;
}

bool GUIState::getIsInTracklet() const
{
    return isInTracklet;
}

void GUIState::setIsInTracklet(bool value)
{
    isInTracklet = value;
}

int GUIState::getSelectedTrackID() const
{
    return selectedTrackID;
}

void GUIState::setSelectedTrackID(int value)
{
    selectedTrackID = value;
}

int GUIState::getStrategyStep() const
{
    return strategyStep;
}

void GUIState::setStrategyStep(int value)
{
    strategyStep = value;
}

int GUIState::getTrackID() const
{
    return trackID;
}

void GUIState::setTrackID(int value)
{
    trackID = value;
}

int GUIState::getObjectID() const
{
    return objectID;
}

void GUIState::setObjectID(int value)
{
    objectID = value;
}


QObject *GUIState::qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return getInstance();
}


}
