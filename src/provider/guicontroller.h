#ifndef GUICONTROLLER_H
#define GUICONTROLLER_H

#include "guistate.h"

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>
#include <QThread>

namespace CellTracker {
class WorkerThread : public QThread {
public:
    void run();
};

class GUIController : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE void changeFrame(int diff);
    Q_INVOKABLE void changeFrameAbs(int newFrame);
    Q_INVOKABLE void changeStrategy(int strat);
    Q_INVOKABLE void changeAction(GUIState::Action act);

    Q_INVOKABLE void startStrategy();

    Q_INVOKABLE void selectCell(int frame, int x, int y);

    static GUIController *getInstance();
    static QObject *qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine);

private:
    explicit GUIController(QObject *parent = 0);
    static GUIController *theInstance;

    WorkerThread worker;
    GUIState::Strategy currentStrategy;

signals:

public slots:
};
}

#endif // GUICONTROLLER_H
