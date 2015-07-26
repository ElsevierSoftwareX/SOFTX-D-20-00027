#ifndef GUICONTROLLER_H
#define GUICONTROLLER_H

#include "guistate.h"

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>
#include <QThread>

namespace CellTracker {
class GUIController : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE void changeFrame(int diff);
    Q_INVOKABLE void changeFrameAbs(int newFrame);
    Q_INVOKABLE void changeStrategy(int strat);
    Q_INVOKABLE void changeAction(int act);

    Q_INVOKABLE int getCurrentStrategy() const;
    Q_INVOKABLE int getCurrentAction() const;

    Q_INVOKABLE void startStrategy();

    Q_INVOKABLE void connectTracks();

    Q_INVOKABLE void hoverCell(int frame, int x, int y);
    Q_INVOKABLE void selectCell(int frame, int x, int y);

     static GUIController *getInstance();
    static QObject *qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine);

private:
    explicit GUIController(QObject *parent = 0);
    static GUIController *theInstance;

    Q_PROPERTY(int currentStrategy READ getCurrentStrategy NOTIFY currentStrategyChanged) GUIState::Strategy currentStrategy = GUIState::Strategy::STRATEGY_DEFAULT;
    Q_PROPERTY(int currentAction READ getCurrentAction NOTIFY currentActionChanged) GUIState::Action currentAction = GUIState::Action::ACTION_DEFAULT;

    /* the strategies */
    void runStrategyClickJump(unsigned int lastNImages, unsigned long lastImageDelay);
    void runStrategyClickSpin(unsigned long delay);
    void runStrategyClickStep(unsigned long delay);
    void runStrategyHoverStep(unsigned long delay);


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
    void currentActionChanged(int);

public slots:
};
}

#endif // GUICONTROLLER_H
