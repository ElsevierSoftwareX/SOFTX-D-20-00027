#ifndef GUISTATE_H
#define GUISTATE_H

#include "project.h"
#include "dataprovider.h"
#include "base/object.h"

#include <memory>

#include <QDebug>
#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>

#define CT_PROP(type, name, capName) \
    private: Q_PROPERTY(type name READ get##capName WRITE set##capName NOTIFY name##Changed) type name; \
    public: Q_INVOKABLE type get##capName () { return name; } \
    public: Q_INVOKABLE void set##capName (type value ) { if (name != value) emit name##Changed(name = value); }

#define CT_PROP_LIMITS(type, name, capName, lower, upper) \
    private: Q_PROPERTY(type name READ get##capName WRITE set##capName NOTIFY name##Changed) type name; \
    public: Q_INVOKABLE type get##capName () { return name; } \
    public: Q_INVOKABLE void set##capName (type value ) { if (name != value) emit name##Changed(name = (value > upper)?upper:(value < lower)?lower:value); }

#define CT_PROP_PTR(type, name, capName) \
    private: Q_PROPERTY(type name READ get##capName WRITE set##capName NOTIFY name##Changed) type name; \
    public: Q_INVOKABLE type get##capName () { return name; } \
    public: Q_INVOKABLE void set##capName (type value ) { if (name.lock() != value.lock()) emit name##Changed(name = value); }

namespace CellTracker {
/*!
 * \brief The GUIState class
 *
 * This class is primarily used to hold many variables that are used in the GUI.
 * It also generates getters, setters and signals for them and registers them with QML.
 */
class GUIState : public QObject
{
    Q_OBJECT

public:
    static GUIState *getInstance();
    static QObject *qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine);

    enum Strategy {
        STRATEGY_DEFAULT = 0,
        STRATEGY_CLICK_STEP,
        STRATEGY_CLICK_JUMP,
        STRATEGY_CLICK_SPIN
    };
    Q_ENUMS(Strategy)

    enum Action {
        ACTION_DEFAULT = 0,
        ACTION_ADD_DAUGHTERS,
        ACTION_ADD_MERGER,
        ACTION_ADD_UNMERGER,
        ACTION_DELETE_CELL,
        ACTION_DELETE_CELLS_FROM,
        ACTION_DELETE_CELLS_TILL
    };
    Q_ENUMS(Action)

    enum ProjType {
        PROJTYPE_HDF5,
        PROJTYPE_XML,
    };
    Q_ENUMS(ProjType)

private:
    explicit GUIState(QObject *parent = 0);
    static GUIState *theInstance;

    CT_PROP(std::shared_ptr<Project>, proj, Proj)
    CT_PROP(QString, projPath, ProjPath)
    CT_PROP(ProjType, projType, ProjType)

    CT_PROP(int, currentFrame, CurrentFrame)
    CT_PROP(int, currentSlice, CurrentSlice)
    CT_PROP(int, currentChannel, CurrentChannel)
    CT_PROP(int, maximumFrame, MaximumFrame)
    CT_PROP(int, maximumSlice, MaximumSlice)
    CT_PROP(int, maximumChannel, MaximumChannel)

    CT_PROP(float, mouseX, MouseX)
    CT_PROP(float, mouseY, MouseY)
    CT_PROP(bool, mouseAreaActive, MouseAreaActive)

    CT_PROP_PTR(std::weak_ptr<Object>, selectedCell, SelectedCell)
    CT_PROP(int, selectedCellID, SelectedCellID)
    CT_PROP(int, selectedCellFrame, SelectedCellFrame)

    CT_PROP_PTR(std::weak_ptr<AutoTracklet>, selectedAutoTrack, SelectedAutoTrack)
    CT_PROP(int, selectedAutoTrackStart, SelectedAutoTrackStart)
    CT_PROP(int, selectedAutoTrackEnd, SelectedAutoTrackEnd)
    CT_PROP(int, selectedAutoTrackLength, SelectedAutoTrackLength)

    CT_PROP_PTR(std::weak_ptr<Tracklet>, selectedTrack, SelectedTrack)
    CT_PROP(int, selectedTrackStart, SelectedTrackStart)
    CT_PROP(int, selectedTrackEnd, SelectedTrackEnd)
    CT_PROP(int, selectedTrackLength, SelectedTrackLength)
    CT_PROP(QString, selectedTrackStatus, SelectedTrackStatus)
    CT_PROP(QString, selectedTrackPrevious, SelectedTrackPrevious)
    CT_PROP(QString, selectedTrackNext, SelectedTrackNext)

    CT_PROP_PTR(std::weak_ptr<Object>, hoveredCell, HoveredCell)
    CT_PROP(int, hoveredCellID, HoveredCellID)
    CT_PROP(int, hoveredCellFrame, hoveredCellFrame)

    CT_PROP_PTR(std::weak_ptr<AutoTracklet>, hoveredAutoTrack, HoveredAutoTrack)
    CT_PROP(int, hoveredAutoTrackStart, HoveredAutoTrackStart)
    CT_PROP(int, hoveredAutoTrackEnd, HoveredAutoTrackEnd)
    CT_PROP(int, hoveredAutoTrackLength, HoveredAutoTrackLength)

    CT_PROP_PTR(std::weak_ptr<Tracklet>, hoveredTrack, HoveredTrack)
    CT_PROP(int, hoveredTrackStart, HoveredTrackStart)
    CT_PROP(int, hoveredTrackEnd, HoveredTrackEnd)
    CT_PROP(int, hoveredTrackLength, HoveredTrackLength)
    CT_PROP(QString, hoveredTrackStatus, HoveredTrackStatus)
    CT_PROP(QString, hoveredTrackPrevious, HoveredTrackPrevious)
    CT_PROP(QString, hoveredTrackNext, HoveredTrackNext)

    CT_PROP(bool, drawTrackletIDs, DrawTrackletIDs)
    CT_PROP(bool, drawAnnotationInfo, DrawAnnotationInfo)
    CT_PROP(bool, drawOutlines, DrawOutlines)
    CT_PROP(bool, drawCutLine, DrawCutLine)
    CT_PROP(bool, drawSeparation, DrawSeparation)
    CT_PROP(bool, drawAggregation, DrawAggregation)
    CT_PROP(bool, drawDeletion, DrawDeletion)
    CT_PROP(bool, drawFlood, DrawFlood)
    CT_PROP(int, startX, StartX)
    CT_PROP(int, startY, StartY)
    CT_PROP(int, endX, EndX)
    CT_PROP(int, endY, EndY)
    CT_PROP_LIMITS(int, thresh, Thresh, 0, 255)

    CT_PROP_LIMITS(float, zoomFactor, ZoomFactor, 0.5, 5)
    CT_PROP(int, offX, OffX)
    CT_PROP(int, offY, OffY)

    CT_PROP(bool, imageReady, ImageReady)
    CT_PROP(QVariantMap, workedOnProject, WorkedOnProject)

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
        if (selectedAutoTrackID != value) {
            setSelectedAutoTrack(GUIState::getInstance()->getProj()->getAutoTracklet(value));
            emit selectedAutoTrackIDChanged(selectedAutoTrackID = value);
        }
    }
    int getSelectedTrackID() { return selectedTrackID; }
    void setSelectedTrackID(int value) {
        if (selectedTrackID != value) {
            setSelectedTrack(GUIState::getInstance()->getProj()->getGenealogy()->getTracklet(value));
            emit selectedTrackIDChanged(selectedTrackID = value);
        }
    }

private:
    Q_PROPERTY(int hoveredAutoTrackID READ getHoveredAutoTrackID WRITE setHoveredAutoTrackID NOTIFY hoveredAutoTrackIDChanged) int hoveredAutoTrackID;
    Q_PROPERTY(int hoveredTrackID READ getHoveredTrackID WRITE setHoveredTrackID NOTIFY hoveredTrackIDChanged) int hoveredTrackID;
public:
    int getHoveredAutoTrackID() { return hoveredAutoTrackID; }
    void setHoveredAutoTrackID(int value) {
        if (hoveredAutoTrackID != value) {
            setHoveredAutoTrack(GUIState::getInstance()->getProj()->getAutoTracklet(value));
            emit hoveredAutoTrackIDChanged(hoveredAutoTrackID = value);
        }
    }
    int getHoveredTrackID() { return hoveredTrackID; }
    void setHoveredTrackID(int value) {
        if (hoveredTrackID != value) {
            setHoveredTrack(GUIState::getInstance()->getProj()->getGenealogy()->getTracklet(value));
            emit hoveredTrackIDChanged(hoveredTrackID = value);
        }
    }

signals:
    void projChanged(std::shared_ptr<Project>);
    void projPathChanged(QString);
    void projTypeChanged(ProjType);

    void currentFrameChanged(int);
    void currentSliceChanged(int);
    void currentChannelChanged(int);
    void maximumFrameChanged(int);
    void maximumSliceChanged(int);
    void maximumChannelChanged(int);

    void mouseXChanged(float);
    void mouseYChanged(float);
    void mouseAreaActiveChanged(bool);

    void selectedCellChanged(std::weak_ptr<Object>);
    void selectedCellIDChanged(int);
    void selectedCellFrameChanged(int);

    void selectedTrackChanged(std::weak_ptr<Tracklet>);
    void selectedTrackIDChanged(int);
    void selectedTrackStartChanged(int);
    void selectedTrackEndChanged(int);
    void selectedTrackLengthChanged(int);
    void selectedTrackStatusChanged(QString);
    void selectedTrackPreviousChanged(QString);
    void selectedTrackNextChanged(QString);

    void selectedAutoTrackChanged(std::weak_ptr<AutoTracklet>);
    void selectedAutoTrackIDChanged(int);
    void selectedAutoTrackStartChanged(int);
    void selectedAutoTrackEndChanged(int);
    void selectedAutoTrackLengthChanged(int);

    void hoveredCellChanged(std::weak_ptr<Object>);
    void hoveredCellIDChanged(int);
    void hoveredCellFrameChanged(int);

    void hoveredTrackChanged(std::weak_ptr<Tracklet>);
    void hoveredTrackIDChanged(int);
    void hoveredTrackStartChanged(int);
    void hoveredTrackEndChanged(int);
    void hoveredTrackLengthChanged(int);
    void hoveredTrackStatusChanged(QString);
    void hoveredTrackPreviousChanged(QString);
    void hoveredTrackNextChanged(QString);

    void hoveredAutoTrackChanged(std::weak_ptr<AutoTracklet>);
    void hoveredAutoTrackIDChanged(int);
    void hoveredAutoTrackStartChanged(int);
    void hoveredAutoTrackEndChanged(int);
    void hoveredAutoTrackLengthChanged(int);

    void drawTrackletIDsChanged(bool);
    void drawAnnotationInfoChanged(bool);
    void drawOutlinesChanged(bool);
    void drawCutLineChanged(bool);
    void drawSeparationChanged(bool);
    void drawAggregationChanged(bool);
    void drawDeletionChanged(bool);
    void drawFloodChanged(bool);
    void startXChanged(int);
    void startYChanged(int);
    void endXChanged(int);
    void endYChanged(int);
    void threshChanged(int);

    void zoomFactorChanged(float);
    void offXChanged(int);
    void offYChanged(int);

    void imageReadyChanged(bool);
    void workedOnProjectChanged(QVariantMap);

    void backingDataChanged();
    };
}
#endif // GUISTATE_H
