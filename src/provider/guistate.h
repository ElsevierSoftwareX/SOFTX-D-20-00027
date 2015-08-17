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
    public: Q_INVOKABLE void set##capName (type value ) { if (name != value) emit name##Changed(name = value); };

#define CT_PROP_LIMITS(type, name, capName, lower, upper) \
    private: Q_PROPERTY(type name READ get##capName WRITE set##capName NOTIFY name##Changed) type name; \
    public: Q_INVOKABLE type get##capName () { return name; }; \
    public: Q_INVOKABLE void set##capName (type value ) { if (name != value) emit name##Changed(name = (value > upper)?upper:(value < lower)?lower:value); };

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
        ACTION_DELETE_CELL,
        ACTION_DELETE_CELLS_FROM,
        ACTION_DELETE_CELLS_TILL
    };
    Q_ENUMS(Action)

private:
    explicit GUIState(QObject *parent = 0);
    static GUIState *theInstance;

    CT_PROP(std::shared_ptr<Project>, proj, Proj)
    CT_PROP(QString, projPath, ProjPath)

    CT_PROP(int, currentFrame, CurrentFrame)
    CT_PROP(int, maximumFrame, MaximumFrame)

    CT_PROP(float, mouseX, MouseX)
    CT_PROP(float, mouseY, MouseY)
    CT_PROP(bool, mouseAreaActive, MouseAreaActive)

    CT_PROP(std::shared_ptr<Object>, selectedCell, SelectedCell)
    CT_PROP(int, selectedCellID, SelectedCellID)

    CT_PROP(std::shared_ptr<AutoTracklet>, selectedAutoTrack, SelectedAutoTrack)
    CT_PROP(int, selectedAutoTrackStart, SelectedAutoTrackStart)
    CT_PROP(int, selectedAutoTrackEnd, SelectedAutoTrackEnd)
    CT_PROP(int, selectedAutoTrackLength, SelectedAutoTrackLength)

    CT_PROP(std::shared_ptr<Tracklet>, selectedTrack, SelectedTrack)
    CT_PROP(int, selectedTrackStart, SelectedTrackStart)
    CT_PROP(int, selectedTrackEnd, SelectedTrackEnd)
    CT_PROP(int, selectedTrackLength, SelectedTrackLength)

    CT_PROP(std::shared_ptr<Object>, hoveredCell, HoveredCell)
    CT_PROP(int, hoveredCellID, HoveredCellID)

    CT_PROP(std::shared_ptr<AutoTracklet>, hoveredAutoTrack, HoveredAutoTrack)
    CT_PROP(int, hoveredAutoTrackStart, HoveredAutoTrackStart)
    CT_PROP(int, hoveredAutoTrackEnd, HoveredAutoTrackEnd)
    CT_PROP(int, hoveredAutoTrackLength, HoveredAutoTrackLength)

    CT_PROP(std::shared_ptr<Tracklet>, hoveredTrack, HoveredTrack)
    CT_PROP(int, hoveredTrackStart, HoveredTrackStart)
    CT_PROP(int, hoveredTrackEnd, HoveredTrackEnd)
    CT_PROP(int, hoveredTrackLength, HoveredTrackLength)

    CT_PROP(bool, drawOutlines, DrawOutlines)

    CT_PROP_LIMITS(float, zoomFactor, ZoomFactor, 0.5, 5)

    QObject *slider;
public:
    Q_INVOKABLE void setSlider(QObject *value) { slider = value; }
    QObject *getSlider() { return slider; }

private:
    Q_PROPERTY(int selectedAutoTrackID READ getSelectedAutoTrackID WRITE setSelectedAutoTrackID NOTIFY selectedAutoTrackIDChanged) int selectedAutoTrackID;
    Q_PROPERTY(int selectedTrackID READ getSelectedTrackID WRITE setSelectedTrackID NOTIFY selectedTrackIDChanged) int selectedTrackID;
public:
    int getSelectedAutoTrackID() { return selectedAutoTrackID; }
    void setSelectedAutoTrackID(int value) {
        selectedAutoTrackID = value;
        setSelectedAutoTrack(GUIState::getInstance()->getProj()->getAutoTracklet(value));
        emit selectedAutoTrackIDChanged(value);
    }
    int getSelectedTrackID() { return selectedTrackID; }
    void setSelectedTrackID(int value) {
        selectedTrackID = value;
        setSelectedTrack(GUIState::getInstance()->getProj()->getGenealogy()->getTracklet(value));
        emit selectedTrackIDChanged(value);
    }

private:
    Q_PROPERTY(int hoveredAutoTrackID READ getHoveredAutoTrackID WRITE setHoveredAutoTrackID NOTIFY hoveredAutoTrackIDChanged) int hoveredAutoTrackID;
    Q_PROPERTY(int hoveredTrackID READ getHoveredTrackID WRITE setHoveredTrackID NOTIFY hoveredTrackIDChanged) int hoveredTrackID;
public:
    int getHoveredAutoTrackID() { return hoveredAutoTrackID; }
    void setHoveredAutoTrackID(int value) {
        hoveredAutoTrackID = value;
        setHoveredAutoTrack(GUIState::getInstance()->getProj()->getAutoTracklet(value));
        emit hoveredAutoTrackIDChanged(value);
    }
    int getHoveredTrackID() { return hoveredTrackID; }
    void setHoveredTrackID(int value) {
        hoveredTrackID = value;
        setHoveredTrack(GUIState::getInstance()->getProj()->getGenealogy()->getTracklet(value));
        emit hoveredTrackIDChanged(value);
    }

signals:
    void projChanged(std::shared_ptr<Project>);
    void projPathChanged(QString);

    void currentFrameChanged(int);
    void maximumFrameChanged(int);

    void mouseXChanged(float);
    void mouseYChanged(float);
    void mouseAreaActiveChanged(bool);

    void selectedCellChanged(std::shared_ptr<Object>);
    void selectedCellIDChanged(int);

    void selectedTrackChanged(std::shared_ptr<Tracklet>);
    void selectedTrackIDChanged(int);
    void selectedTrackStartChanged(int);
    void selectedTrackEndChanged(int);
    void selectedTrackLengthChanged(int);

    void selectedAutoTrackChanged(std::shared_ptr<AutoTracklet>);
    void selectedAutoTrackIDChanged(int);
    void selectedAutoTrackStartChanged(int);
    void selectedAutoTrackEndChanged(int);
    void selectedAutoTrackLengthChanged(int);

    void hoveredCellChanged(std::shared_ptr<Object>);
    void hoveredCellIDChanged(int);

    void hoveredTrackChanged(std::shared_ptr<Tracklet>);
    void hoveredTrackIDChanged(int);
    void hoveredTrackStartChanged(int);
    void hoveredTrackEndChanged(int);
    void hoveredTrackLengthChanged(int);

    void hoveredAutoTrackChanged(std::shared_ptr<AutoTracklet>);
    void hoveredAutoTrackIDChanged(int);
    void hoveredAutoTrackStartChanged(int);
    void hoveredAutoTrackEndChanged(int);
    void hoveredAutoTrackLengthChanged(int);

    void drawOutlinesChanged(bool);
    void zoomFactorChanged(float);

    void backingDataChanged();
    };
}
#endif // GUISTATE_H
