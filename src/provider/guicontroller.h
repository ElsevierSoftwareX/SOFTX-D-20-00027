#ifndef GUICONTROLLER_H
#define GUICONTROLLER_H

#include "guistate.h"

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>
#include <QThread>

namespace CellTracker {
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

    Q_INVOKABLE void hoverCell(int frame, int x, int y);
    Q_INVOKABLE void selectCell(int frame, int x, int y);

    static GUIController *getInstance();
    static QObject *qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine);

private:
    explicit GUIController(QObject *parent = 0);
    static GUIController *theInstance;
    bool abortStrategyIssued;

    Q_PROPERTY(int currentStrategy
               READ getCurrentStrategy
               WRITE setCurrentStrategy
               NOTIFY currentStrategyChanged) GUIState::Strategy currentStrategy = GUIState::Strategy::STRATEGY_DEFAULT;
    Q_PROPERTY(bool currentStrategyRunning
               READ getCurrentStrategyRunning
               WRITE setCurrentStrategyRunning
               NOTIFY currentStrategyRunningChanged) bool currentStrategyRunning = false;
    Q_PROPERTY(int currentAction
               READ getCurrentAction
               WRITE setCurrentAction
               NOTIFY currentActionChanged) GUIState::Action currentAction = GUIState::Action::ACTION_DEFAULT;

    /* the strategies */
    void runStrategyClickJump(unsigned long delay, unsigned int show);
    void runStrategyClickSpin(unsigned long delay);
    void runStrategyClickStep(unsigned long delay);


    /* helper functions for hovering a Cell and (Auto)Track(lets) */
    void hoverCell(std::shared_ptr<Object> o);
    void unhoverCell();
    void hoverTrack(std::shared_ptr<Object> o, std::shared_ptr<Project> proj);
    void unhoverTrack();
    void hoverAutoTracklet(std::shared_ptr<Object> o, std::shared_ptr<Project> proj);
    void unhoverAutoTracklet();

    /* helper functions for selecting a Cell and (Auto)Track(lets) */
    void selectCell(std::shared_ptr<Object> o);
    void deselectCell();
    void selectTrack(std::shared_ptr<Object> o, std::shared_ptr<Project> proj);
    void deselectTrack();
    void selectAutoTracklet(std::shared_ptr<Object> o, std::shared_ptr<Project> proj);
    void deselectAutoTracklet();

signals:
    void currentStrategyChanged(int);
    void currentStrategyRunningChanged(bool);
    void currentActionChanged(int);

public slots:
};
}

#endif // GUICONTROLLER_H
