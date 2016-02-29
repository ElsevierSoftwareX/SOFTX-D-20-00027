#include "messagerelay.h"

namespace CellTracker {
MessageRelay *MessageRelay::instance = nullptr;

MessageRelay::MessageRelay() {}

/*!
 * \brief returns an instance of the MessageRelay
 * \return an instance of the MessageRelay
 */
MessageRelay *MessageRelay::getInstance()
{
    if (instance == nullptr)
        MessageRelay::instance = new MessageRelay();

    return instance;
}

/*!
 * \brief provides an instance of MessageRelay for use in QML
 * \param engine (unused)
 * \param scriptEngine (unused)
 * \return a pointer to the instance of MessageRelay
 */
QObject *MessageRelay::qmlInstanceProvider(QQmlEngine *engine, QJSEngine *scriptEngine) {
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return getInstance();
}
}
