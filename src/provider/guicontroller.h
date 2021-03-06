/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2017, 2016, 2015 Sebastian Wagner
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
#ifndef GUICONTROLLER_H
#define GUICONTROLLER_H

#include "guistate.h"

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>
#include <QThread>

namespace TraCurate {
/*!
 * \brief The GUIController class
 *
 * This Class is used by the GUI to invoke changes on the Datastructure
 * and to implement Strategies that control the GUI.
 */
class GUIController : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE void changeFrame(int diff);
    Q_INVOKABLE void changeFrameAbs(int newFrame);
    Q_INVOKABLE void changeSlice(int newSlice);
    Q_INVOKABLE void changeChannel(int newChannel);

    Q_INVOKABLE int getCurrentStrategy() const;
    Q_INVOKABLE bool getCurrentStrategyRunning() const;
    Q_INVOKABLE int getCurrentAction() const;
    Q_INVOKABLE void setCurrentStrategy(int value);
    Q_INVOKABLE void setCurrentStrategyRunning(bool value);
    Q_INVOKABLE void setCurrentAction(int value);

    /* control the running of the current strategy */
    Q_INVOKABLE void startStrategy(unsigned long delay, unsigned int show);
    Q_INVOKABLE void abortStrategy();

    Q_INVOKABLE bool connectTracks();

    Q_INVOKABLE void hoverCell(int frame, int slice, int channel, double x, double y);
    Q_INVOKABLE void selectCell(int frame, int slice, int channel, double x, double y);

    Q_INVOKABLE void selectLastCellByTrackId(int trackId);

    Q_INVOKABLE void changeStatus(int trackId, int status);

    Q_INVOKABLE void cutObject(int startX, int startY, int endX, int endY);
    Q_INVOKABLE void mergeObjects(int firstX, int firstY, int secondX, int secondY);
    Q_INVOKABLE void deleteObject(double posX, double posY);
    Q_INVOKABLE void floodFill(int posX, int posY);

    static GUIController *getInstance();
    static QObject *qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine);

    void waitForFutures();

private:
    explicit GUIController(QObject *parent = 0);
    static GUIController *theInstance;
    bool abortStrategyIssued;

    Q_PROPERTY(int currentStrategy
               READ getCurrentStrategy
               WRITE setCurrentStrategy
               NOTIFY currentStrategyChanged) GUIState::Strategy currentStrategy;
    Q_PROPERTY(bool currentStrategyRunning
               READ getCurrentStrategyRunning
               WRITE setCurrentStrategyRunning
               NOTIFY currentStrategyRunningChanged) bool currentStrategyRunning;
    Q_PROPERTY(int currentAction
               READ getCurrentAction
               WRITE setCurrentAction
               NOTIFY currentActionChanged) GUIState::Action currentAction;

    /* the strategies */
    void runStrategyClickJump(unsigned long delay, unsigned int show);
    void runStrategyClickSpin(unsigned long delay);
    void runStrategyClickStep(unsigned long delay);


    /* helper functions for hovering a Cell and (Auto)Track(lets) */
    void hoverCell(std::shared_ptr<Object> const &o);
    void unhoverCell();
    void hoverTrack(std::shared_ptr<Object> const &o, std::shared_ptr<Project> const &proj);
    void unhoverTrack();
    void hoverAutoTracklet(std::shared_ptr<Object> const &o, std::shared_ptr<Project> const &proj);
    void unhoverAutoTracklet();

    /* helper functions for selecting a Cell and (Auto)Track(lets) */
    void selectCell(std::shared_ptr<Object> const &o);
    void deselectCell();
    void selectTrack(std::shared_ptr<Object> const &o, std::shared_ptr<Project> const &proj);
    void deselectTrack();
    void selectAutoTracklet(std::shared_ptr<Object> const &o, std::shared_ptr<Project> const &proj);
    void deselectAutoTracklet();

    QList<QFuture<void>> futures;
signals:
    void currentStrategyChanged(int);
    void currentStrategyRunningChanged(bool);
    void currentActionChanged(int);

public slots:
};
}

#endif // GUICONTROLLER_H
