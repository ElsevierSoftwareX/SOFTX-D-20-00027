/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2016, 2015 Sebastian Wagner
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
#include "guistate.h"

#include <QDebug>

namespace TraCurate {

GUIState *GUIState::theInstance = nullptr;

GUIState *GUIState::getInstance(){
    if (!theInstance)
        theInstance = new GUIState();
    return theInstance;
}

GUIState::GUIState(QObject *parent) : QObject(parent),
    proj(nullptr),
    projPath(""),
    needsSave(false),
    currentFrame(0),
    currentSlice(0),
    currentChannel(0),
    maximumFrame(0),
    maximumSlice(0),
    maximumChannel(0),
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
    drawFlood(false),
    startX(-1),
    startY(-1),
    endX(-1),
    endY(-1),
    thresh(5),
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
