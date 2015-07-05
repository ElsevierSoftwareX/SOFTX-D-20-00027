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
    void changeFrame(int newFrame);
    void changeStrategy(GUIState::Strategy strat);
    void changeAction(GUIState::Action act);

    static GUIController *getInstance();
    static QObject *qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine);

private:
    explicit GUIController(QObject *parent = 0);
    static GUIController *theInstance;
    QThread worker;

signals:

public slots:
};
}

#endif // GUICONTROLLER_H
