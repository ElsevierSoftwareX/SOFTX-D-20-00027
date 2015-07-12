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
        ACTION_ADD_DAUGHTERS
    };
    Q_ENUMS(Action)

private:
    explicit GUIState(QObject *parent = 0);
    static GUIState *theInstance;

    CT_PROP(std::shared_ptr<Project>, proj, Proj)
    CT_PROP(std::shared_ptr<Object>, lastObject, LastObject)
    CT_PROP(std::shared_ptr<Object>, currentCell, CurrentCell)
    CT_PROP(std::shared_ptr<Object>, selectedCell, SelectedCell)
    CT_PROP(std::shared_ptr<Object>, motherCell, MotherCell)
    CT_PROP(QList<std::shared_ptr<Object>>, daughterCells, DaughterCells)
    CT_PROP(QList<std::shared_ptr<Object>>, setListOfPolygons, SetListOfPolygons)

    CT_PROP(int, selectedCellID, SelectedCellID)

    /* form imageProvider */
    CT_PROP(int, objectID, ObjectID)
    CT_PROP(int, trackID, TrackID)
    CT_PROP(int, currentFrame, CurrentFrame)
    CT_PROP(int, strategyStep, StrategyStep)
    CT_PROP(int, selectedTrackID, SelectedTrackID)
    CT_PROP(bool, isInTracklet, IsInTracklet)

    /* from mousePosition */
    CT_PROP(int, maximumValue, MaximumValue)
    CT_PROP(float, lastX, LastX)
    CT_PROP(float, lastY, LastY)
    CT_PROP(float, sliderValue, SliderValue)
    CT_PROP(QString, strategy, Strategy)
    CT_PROP(QString, jumpStrategy, JumpStrategy)
    CT_PROP(QString, mouseAction, MouseAction)
    CT_PROP(bool, mouseAreaActive, MouseAreaActive)
    CT_PROP(QString, status, Status)
    CT_PROP(QString, path, Path)

    /* from cellImage */
    CT_PROP(bool, isSelectedInTracklet, IsSelectedInTracklet)
    CT_PROP(int, cellID, CellID)
    CT_PROP(int, trackStart, TrackStart)
    CT_PROP(int, trackEnd, TrackEnd)
    CT_PROP(int, trackLength, TrackLength)
    CT_PROP(int, frameID, FrameID)
    CT_PROP(int, selectedTrackStart, SelectedTrackStart)
    CT_PROP(int, selectedTrackEnd, SelectedTrackEnd)
    CT_PROP(int, selectedTrackLength, SelectedTrackLength)
    CT_PROP(int, jumpTrackEnd, JumpTrackEnd)
    CT_PROP(int, frames, Frames)
    CT_PROP(float, delay, Delay)



signals:
    void projChanged(std::shared_ptr<Project>);
    void lastObjectChanged(std::shared_ptr<Object>);
    void currentCellChanged(std::shared_ptr<Object>);
    void selectedCellChanged(std::shared_ptr<Object>);
    void motherCellChanged(std::shared_ptr<Object>);
    void daughterCellsChanged(QList<std::shared_ptr<Object>>);
    void setListOfPolygonsChanged(QList<std::shared_ptr<Object>>);
    void selectedCellIDChanged(int);

    void objectIDChanged(int);
    void trackIDChanged(int);
    void currentFrameChanged(int);
    void strategyStepChanged(int);
    void selectedTrackIDChanged(int);
    void isInTrackletChanged(bool);

    void maximumValueChanged(int);
    void lastXChanged(float);
    void lastYChanged(float);
    void sliderValueChanged(float);
    void strategyChanged(QString);
    void jumpStrategyChanged(QString);
    void mouseActionChanged(QString);
    void mouseAreaActiveChanged(bool);
    void statusChanged(QString);
    void pathChanged(QString);

    void isSelectedInTrackletChanged(bool);
    void cellIDChanged(int);
    void trackStartChanged(int);
    void trackEndChanged(int);
    void trackLengthChanged(int);
    void frameIDChanged(int);
    void selectedTrackStartChanged(int);
    void selectedTrackEndChanged(int);
    void selectedTrackLengthChanged(int);
    void jumpTrackEndChanged(int);
    void framesChanged(int);
    void delayChanged(float);

    /* new variables, the old ones should go */
    CT_PROP(int, newCurrentFrame, NewCurrentFrame)

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
private:
    Q_PROPERTY(int newSelectedAutoTrackID READ getNewSelectedAutoTrackID WRITE setNewSelectedAutoTrackID NOTIFY newSelectedAutoTrackIDChanged) int newSelectedAutoTrackID;
    Q_PROPERTY(int newSelectedTrackID READ getNewSelectedTrackID WRITE setNewSelectedTrackID NOTIFY newSelectedTrackIDChanged) int newSelectedTrackID;
public:
    int getNewSelectedAutoTrackID() { return newSelectedAutoTrackID; }
    void setNewSelectedAutoTrackID(int value) {
        newSelectedAutoTrackID = value;
        setNewSelectedAutoTrack(DataProvider::getInstance()->getProj()->getAutoTracklet(value));
        emit newSelectedAutoTrackIDChanged(value);
    }
    int getNewSelectedTrackID() { return newSelectedTrackID; }
    void setNewSelectedTrackID(int value) {
        newSelectedTrackID = value;
        setNewSelectedTrack(DataProvider::getInstance()->getProj()->getGenealogy()->getTracklet(value));
        emit newSelectedTrackIDChanged(value);
    }

signals:
    void newCurrentFrameChanged(int);

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
    };
}

#endif // GUISTATE_H
