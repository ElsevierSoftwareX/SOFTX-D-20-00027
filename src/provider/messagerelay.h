#ifndef MESSAGERELAY_H
#define MESSAGERELAY_H

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>

class MessageRelay : public QObject
{
    Q_OBJECT

signals:
    void updateOverallName(QString text);
    void updateOverallMax(int newMax);
    void increaseOverall();

    void updateDetailName(QString text);
    void updateDetailMax(int newMax);
    void increaseDetail();

    void finishNotification();

    void updateStatusBar(QString message);

public:
    static MessageRelay *getInstance();
    static QObject *qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine);

    static void emitUpdateOverallName(QString text) { MessageRelay::getInstance()->updateOverallName(text); }
    static void emitUpdateOverallMax(int newMax) { MessageRelay::getInstance()->updateOverallMax(newMax); }
    static void emitIncreaseOverall() { MessageRelay::getInstance()->increaseOverall(); }

    static void emitUpdateDetailName(QString text) { MessageRelay::getInstance()->updateDetailName(text); }
    static void emitUpdateDetailMax(int newMax) { MessageRelay::getInstance()->updateDetailMax(newMax); }
    static void emitIncreaseDetail() { MessageRelay::getInstance()->increaseDetail(); }

    static void emitFinishNotification() { MessageRelay::getInstance()->finishNotification(); }

    static void emitUpdateStatusBar(QString message) { MessageRelay::getInstance()->updateStatusBar(message); }

private:
    MessageRelay();

    static MessageRelay *instance;
};

#endif // MESSAGERELAY_H
