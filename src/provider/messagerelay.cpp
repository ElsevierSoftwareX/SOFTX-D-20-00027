#include "messagerelay.h"


MessageRelay *MessageRelay::instance = nullptr;

MessageRelay::MessageRelay() { }

MessageRelay *MessageRelay::getInstance()
{
    if (instance == nullptr)
        MessageRelay::instance = new MessageRelay();

    return instance;
}

QObject *MessageRelay::qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return getInstance();
}
