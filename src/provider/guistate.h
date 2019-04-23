/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2017, 2016, 2015 Konstantin Thierbach, Sebastian Wagner
 *
 * TraCurate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TraCurate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with TraCurate.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef GUISTATE_H
#define GUISTATE_H

#include "project.h"
#include "dataprovider.h"
#include "base/object.h"

#include <cmath>
#include <memory>

#include <QDebug>
#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>

#define TC_PROP(type, name, capName) \
    private: Q_PROPERTY(type name READ get##capName WRITE set##capName NOTIFY name##Changed) type name; \
    public: Q_INVOKABLE type get##capName () { return name; } \
    public: Q_INVOKABLE void set##capName (type value ) { if (name != value) emit name##Changed(name = value); }

#define TC_PROP_DBL(type, name, capName) \
    private: Q_PROPERTY(type name READ get##capName WRITE set##capName NOTIFY name##Changed) type name; \
    public: Q_INVOKABLE type get##capName () { return name; } \
    public: Q_INVOKABLE void set##capName (type value ) { if (fabs(name - value) > 0) emit name##Changed(name = value); }

#define TC_PROP_LIMITS(type, name, capName, lower, upper) \
    private: Q_PROPERTY(type name READ get##capName WRITE set##capName NOTIFY name##Changed) type name; \
    public: Q_INVOKABLE type get##capName () { return name; } \
    public: Q_INVOKABLE void set##capName (type value ) { if (name != value) emit name##Changed(name = (value > upper)?upper:(value < lower)?lower:value); }

#define TC_PROP_LIMITS_DBL(type, name, capName, lower, upper) \
    private: Q_PROPERTY(type name READ get##capName WRITE set##capName NOTIFY name##Changed) type name; \
    public: Q_INVOKABLE type get##capName () { return name; } \
    public: Q_INVOKABLE void set##capName (type value ) { if (fabs(name - value) > 0) emit name##Changed(name = (value > upper)?upper:(value < lower)?lower:value); }

#define TC_PROP_PTR(type, name, capName) \
    private: Q_PROPERTY(type name READ get##capName WRITE set##capName NOTIFY name##Changed) type name; \
    public: Q_INVOKABLE type get##capName () { return name; } \
    public: Q_INVOKABLE void set##capName (type value ) { if (name.lock() != value.lock()) emit name##Changed(name = value); }

namespace TraCurate {
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

    TC_PROP(std::shared_ptr<Project>, proj, Proj)
    TC_PROP(QString, projPath, ProjPath)
    TC_PROP(ProjType, projType, ProjType)

    TC_PROP(int, currentFrame, CurrentFrame)
    TC_PROP(int, currentSlice, CurrentSlice)
    TC_PROP(int, currentChannel, CurrentChannel)
    TC_PROP(int, maximumFrame, MaximumFrame)
    TC_PROP(int, maximumSlice, MaximumSlice)
    TC_PROP(int, maximumChannel, MaximumChannel)

    TC_PROP_DBL(double, mouseX, MouseX)
    TC_PROP_DBL(double, mouseY, MouseY)
    TC_PROP(bool, mouseAreaActive, MouseAreaActive)

    TC_PROP_PTR(std::weak_ptr<Object>, selectedCell, SelectedCell)
    TC_PROP(int, selectedCellID, SelectedCellID)
    TC_PROP(int, selectedCellFrame, SelectedCellFrame)

    TC_PROP_PTR(std::weak_ptr<AutoTracklet>, selectedAutoTrack, SelectedAutoTrack)
    TC_PROP(int, selectedAutoTrackStart, SelectedAutoTrackStart)
    TC_PROP(int, selectedAutoTrackEnd, SelectedAutoTrackEnd)
    TC_PROP(int, selectedAutoTrackLength, SelectedAutoTrackLength)

    TC_PROP_PTR(std::weak_ptr<Tracklet>, selectedTrack, SelectedTrack)
    TC_PROP(int, selectedTrackStart, SelectedTrackStart)
    TC_PROP(int, selectedTrackEnd, SelectedTrackEnd)
    TC_PROP(int, selectedTrackLength, SelectedTrackLength)
    TC_PROP(QString, selectedTrackStatus, SelectedTrackStatus)
    TC_PROP(QString, selectedTrackPrevious, SelectedTrackPrevious)
    TC_PROP(QString, selectedTrackNext, SelectedTrackNext)

    TC_PROP_PTR(std::weak_ptr<Object>, hoveredCell, HoveredCell)
    TC_PROP(int, hoveredCellID, HoveredCellID)
    TC_PROP(int, hoveredCellFrame, hoveredCellFrame)

    TC_PROP_PTR(std::weak_ptr<AutoTracklet>, hoveredAutoTrack, HoveredAutoTrack)
    TC_PROP(int, hoveredAutoTrackStart, HoveredAutoTrackStart)
    TC_PROP(int, hoveredAutoTrackEnd, HoveredAutoTrackEnd)
    TC_PROP(int, hoveredAutoTrackLength, HoveredAutoTrackLength)

    TC_PROP_PTR(std::weak_ptr<Tracklet>, hoveredTrack, HoveredTrack)
    TC_PROP(int, hoveredTrackStart, HoveredTrackStart)
    TC_PROP(int, hoveredTrackEnd, HoveredTrackEnd)
    TC_PROP(int, hoveredTrackLength, HoveredTrackLength)
    TC_PROP(QString, hoveredTrackStatus, HoveredTrackStatus)
    TC_PROP(QString, hoveredTrackPrevious, HoveredTrackPrevious)
    TC_PROP(QString, hoveredTrackNext, HoveredTrackNext)

    TC_PROP(bool, drawTrackletIDs, DrawTrackletIDs)
    TC_PROP(bool, drawAnnotationInfo, DrawAnnotationInfo)
    TC_PROP(bool, drawOutlines, DrawOutlines)
    TC_PROP(bool, drawCutLine, DrawCutLine)
    TC_PROP(bool, drawSeparation, DrawSeparation)
    TC_PROP(bool, drawAggregation, DrawAggregation)
    TC_PROP(bool, drawDeletion, DrawDeletion)
    TC_PROP(bool, drawFlood, DrawFlood)
    TC_PROP(int, startX, StartX)
    TC_PROP(int, startY, StartY)
    TC_PROP(int, endX, EndX)
    TC_PROP(int, endY, EndY)
    TC_PROP_LIMITS(int, thresh, Thresh, 0, 255)

    TC_PROP_LIMITS_DBL(double, zoomFactor, ZoomFactor, 0.5, 5.0)
    TC_PROP(int, offX, OffX)
    TC_PROP(int, offY, OffY)

    TC_PROP(bool, imageReady, ImageReady)
    TC_PROP(QVariantMap, workedOnProject, WorkedOnProject)

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

    void mouseXChanged(double);
    void mouseYChanged(double);
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

    void zoomFactorChanged(double);
    void offXChanged(int);
    void offYChanged(int);

    void imageReadyChanged(bool);
    void workedOnProjectChanged(QVariantMap);

    void backingDataChanged();
    };
}
#endif // GUISTATE_H
