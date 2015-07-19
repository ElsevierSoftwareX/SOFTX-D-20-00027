#ifndef GUISTATE_H
#define GUISTATE_H

#include "project.h"
#include "dataprovider.h"
#include "base_data/object.h"

#include <memory>

#include <QDebug>
#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>

#define CT_PROP(type, name, capName) \
    private: Q_PROPERTY(type name READ get##capName WRITE set##capName NOTIFY name##Changed) type name; \
    public: Q_INVOKABLE type get##capName () { return name; }; \
    public: Q_INVOKABLE void set##capName (type value ) { name = value; emit name##Changed(value); };

namespace CellTracker {

class GUIState : public QObject
{
    Q_OBJECT

public:
    static GUIState *getInstance();
    static QObject *qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine);

    enum Strategy {
        STRATEGY_DEFAULT,
        STRATEGY_CLICK_STEP,
        STRATEGY_HOVER_STEP,
        STRATEGY_CLICK_JUMP,
        STRATEGY_CLICK_SPIN
    };
    Q_ENUMS(Strategy)

    enum Action {
        ACTION_DEFAULT,
        ACTION_ADD_DAUGHTERS,
        ACTION_DELETE_CELL
    };
    Q_ENUMS(Action)

private:
    explicit GUIState(QObject *parent = 0);
    static GUIState *theInstance;

    CT_PROP(std::shared_ptr<Project>, newProj, NewProj)
    CT_PROP(QString, newProjPath, NewProjPath)

    CT_PROP(int, newCurrentFrame, NewCurrentFrame)
    CT_PROP(int, newMaximumFrame, NewMaximumFrame)

    CT_PROP(float, newMouseX, NewMouseX)
    CT_PROP(float, newMouseY, NewMouseY)
    CT_PROP(bool, newMouseAreaActive, NewMouseAreaActive)

    CT_PROP(std::shared_ptr<Object>, newSelectedCell, NewSelectedCell)
    CT_PROP(int, newSelectedCellID, NewSelectedCellID)

    CT_PROP(std::shared_ptr<AutoTracklet>, newSelectedAutoTrack, NewSelectedAutoTrack)
    CT_PROP(int, newSelectedAutoTrackStart, NewSelectedAutoTrackStart)
    CT_PROP(int, newSelectedAutoTrackEnd, NewSelectedAutoTrackEnd)
    CT_PROP(int, newSelectedAutoTrackLength, NewSelectedAutoTrackLength)

    CT_PROP(std::shared_ptr<Tracklet>, newSelectedTrack, NewSelectedTrack)
    CT_PROP(int, newSelectedTrackStart, NewSelectedTrackStart)
    CT_PROP(int, newSelectedTrackEnd, NewSelectedTrackEnd)
    CT_PROP(int, newSelectedTrackLength, NewSelectedTrackLength)

    CT_PROP(std::shared_ptr<Object>, newHoveredCell, NewHoveredCell)
    CT_PROP(int, newHoveredCellID, NewHoveredCellID)

    CT_PROP(std::shared_ptr<AutoTracklet>, newHoveredAutoTrack, NewHoveredAutoTrack)
    CT_PROP(int, newHoveredAutoTrackStart, NewHoveredAutoTrackStart)
    CT_PROP(int, newHoveredAutoTrackEnd, NewHoveredAutoTrackEnd)
    CT_PROP(int, newHoveredAutoTrackLength, NewHoveredAutoTrackLength)

    CT_PROP(std::shared_ptr<Tracklet>, newHoveredTrack, NewHoveredTrack)
    CT_PROP(int, newHoveredTrackStart, NewHoveredTrackStart)
    CT_PROP(int, newHoveredTrackEnd, NewHoveredTrackEnd)
    CT_PROP(int, newHoveredTrackLength, NewHoveredTrackLength)

private:
    Q_PROPERTY(int newSelectedAutoTrackID READ getNewSelectedAutoTrackID WRITE setNewSelectedAutoTrackID NOTIFY newSelectedAutoTrackIDChanged) int newSelectedAutoTrackID;
    Q_PROPERTY(int newSelectedTrackID READ getNewSelectedTrackID WRITE setNewSelectedTrackID NOTIFY newSelectedTrackIDChanged) int newSelectedTrackID;
public:
    int getNewSelectedAutoTrackID() { return newSelectedAutoTrackID; }
    void setNewSelectedAutoTrackID(int value) {
        newSelectedAutoTrackID = value;
        setNewSelectedAutoTrack(GUIState::getInstance()->getNewProj()->getAutoTracklet(value));
        emit newSelectedAutoTrackIDChanged(value);
    }
    int getNewSelectedTrackID() { return newSelectedTrackID; }
    void setNewSelectedTrackID(int value) {
        newSelectedTrackID = value;
        setNewSelectedTrack(GUIState::getInstance()->getNewProj()->getGenealogy()->getTracklet(value));
        emit newSelectedTrackIDChanged(value);
    }

private:
    Q_PROPERTY(int newHoveredAutoTrackID READ getNewHoveredAutoTrackID WRITE setNewHoveredAutoTrackID NOTIFY newHoveredAutoTrackIDChanged) int newHoveredAutoTrackID;
    Q_PROPERTY(int newHoveredTrackID READ getNewHoveredTrackID WRITE setNewHoveredTrackID NOTIFY newHoveredTrackIDChanged) int newHoveredTrackID;
public:
    int getNewHoveredAutoTrackID() { return newHoveredAutoTrackID; }
    void setNewHoveredAutoTrackID(int value) {
        newHoveredAutoTrackID = value;
        setNewHoveredAutoTrack(GUIState::getInstance()->getNewProj()->getAutoTracklet(value));
        emit newHoveredAutoTrackIDChanged(value);
    }
    int getNewHoveredTrackID() { return newHoveredTrackID; }
    void setNewHoveredTrackID(int value) {
        newHoveredTrackID = value;
        setNewHoveredTrack(GUIState::getInstance()->getNewProj()->getGenealogy()->getTracklet(value));
        emit newHoveredTrackIDChanged(value);
    }

signals:
    void newProjChanged(std::shared_ptr<Project>);
    void newProjPathChanged(QString);

    void newCurrentFrameChanged(int);
    void newMaximumFrameChanged(int);

    void newMouseXChanged(float);
    void newMouseYChanged(float);
    void newMouseAreaActiveChanged(bool);

    void newSelectedCellChanged(std::shared_ptr<Object>);
    void newSelectedCellIDChanged(int);

    void newSelectedTrackChanged(std::shared_ptr<Tracklet>);
    void newSelectedTrackIDChanged(int);
    void newSelectedTrackStartChanged(int);
    void newSelectedTrackEndChanged(int);
    void newSelectedTrackLengthChanged(int);

    void newSelectedAutoTrackChanged(std::shared_ptr<AutoTracklet>);
    void newSelectedAutoTrackIDChanged(int);
    void newSelectedAutoTrackStartChanged(int);
    void newSelectedAutoTrackEndChanged(int);
    void newSelectedAutoTrackLengthChanged(int);

    void newHoveredCellChanged(std::shared_ptr<Object>);
    void newHoveredCellIDChanged(int);

    void newHoveredTrackChanged(std::shared_ptr<Tracklet>);
    void newHoveredTrackIDChanged(int);
    void newHoveredTrackStartChanged(int);
    void newHoveredTrackEndChanged(int);
    void newHoveredTrackLengthChanged(int);

    void newHoveredAutoTrackChanged(std::shared_ptr<AutoTracklet>);
    void newHoveredAutoTrackIDChanged(int);
    void newHoveredAutoTrackStartChanged(int);
    void newHoveredAutoTrackEndChanged(int);
    void newHoveredAutoTrackLengthChanged(int);
    };
}

//    CT_PROP(std::shared_ptr<Project>, proj, Proj)
//    CT_PROP(std::shared_ptr<Object>, lastObject, LastObject)
//    CT_PROP(std::shared_ptr<Object>, currentCell, CurrentCell)
//    CT_PROP(std::shared_ptr<Object>, selectedCell, SelectedCell)
//    CT_PROP(std::shared_ptr<Object>, motherCell, MotherCell)
//    CT_PROP(QList<std::shared_ptr<Object>>, daughterCells, DaughterCells)
//    CT_PROP(QList<std::shared_ptr<Object>>, setListOfPolygons, SetListOfPolygons)

//    CT_PROP(int, selectedCellID, SelectedCellID)

//    /* form imageProvider */
//    CT_PROP(int, objectID, ObjectID)
//    CT_PROP(int, trackID, TrackID)
//    CT_PROP(int, currentFrame, CurrentFrame)
//    CT_PROP(int, strategyStep, StrategyStep)
//    CT_PROP(int, selectedTrackID, SelectedTrackID)
//    CT_PROP(bool, isInTracklet, IsInTracklet)

//    /* from mousePosition */
//    CT_PROP(int, maximumValue, MaximumValue)
//    CT_PROP(float, lastX, LastX)
//    CT_PROP(float, lastY, LastY)
//    CT_PROP(float, sliderValue, SliderValue)
//    CT_PROP(QString, strategy, Strategy)
//    CT_PROP(QString, jumpStrategy, JumpStrategy)
//    CT_PROP(QString, mouseAction, MouseAction)
//    CT_PROP(bool, mouseAreaActive, MouseAreaActive)
//    CT_PROP(QString, status, Status)
//    CT_PROP(QString, path, Path)

//    /* from cellImage */
//    CT_PROP(bool, isSelectedInTracklet, IsSelectedInTracklet)
//    CT_PROP(int, cellID, CellID)
//    CT_PROP(int, trackStart, TrackStart)
//    CT_PROP(int, trackEnd, TrackEnd)
//    CT_PROP(int, trackLength, TrackLength)
//    CT_PROP(int, frameID, FrameID)
//    CT_PROP(int, selectedTrackStart, SelectedTrackStart)
//    CT_PROP(int, selectedTrackEnd, SelectedTrackEnd)
//    CT_PROP(int, selectedTrackLength, SelectedTrackLength)
//    CT_PROP(int, jumpTrackEnd, JumpTrackEnd)
//    CT_PROP(int, frames, Frames)
//    CT_PROP(float, delay, Delay)



//signals:
//    void projChanged(std::shared_ptr<Project>);
//    void lastObjectChanged(std::shared_ptr<Object>);
//    void currentCellChanged(std::shared_ptr<Object>);
//    void selectedCellChanged(std::shared_ptr<Object>);
//    void motherCellChanged(std::shared_ptr<Object>);
//    void daughterCellsChanged(QList<std::shared_ptr<Object>>);
//    void setListOfPolygonsChanged(QList<std::shared_ptr<Object>>);
//    void selectedCellIDChanged(int);

//    void objectIDChanged(int);
//    void trackIDChanged(int);
//    void currentFrameChanged(int);
//    void strategyStepChanged(int);
//    void selectedTrackIDChanged(int);
//    void isInTrackletChanged(bool);

//    void maximumValueChanged(int);
//    void lastXChanged(float);
//    void lastYChanged(float);
//    void sliderValueChanged(float);
//    void strategyChanged(QString);
//    void jumpStrategyChanged(QString);
//    void mouseActionChanged(QString);
//    void mouseAreaActiveChanged(bool);
//    void statusChanged(QString);
//    void pathChanged(QString);

//    void isSelectedInTrackletChanged(bool);
//    void cellIDChanged(int);
//    void trackStartChanged(int);
//    void trackEndChanged(int);
//    void trackLengthChanged(int);
//    void frameIDChanged(int);
//    void selectedTrackStartChanged(int);
//    void selectedTrackEndChanged(int);
//    void selectedTrackLengthChanged(int);
//    void jumpTrackEndChanged(int);
//    void framesChanged(int);
//    void delayChanged(float);

//    CT_PROP(int, objectID, ObjectID)
//    void objectIDChanged(int);
//    CT_PROP(QList<std::shared_ptr<Object>>, daughterCells, DaughterCells)
//    CT_PROP(std::shared_ptr<Object>, currentCell, CurrentCell)
//    CT_PROP(QString, status, Status)
//    CT_PROP(int, strategyStep, StrategyStep)
//    void daughterCellsChanged(QList<std::shared_ptr<Object>>);
//    void currentCellChanged(std::shared_ptr<Object>);
//    void statusChanged(QString);
//    void strategyStepChanged(int);

#endif // GUISTATE_H
