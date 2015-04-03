#include "messagerelay.h"


MessageRelay *MessageRelay::instance = nullptr;

MessageRelay *MessageRelay::getInstance()
{
    if (instance == nullptr)
        MessageRelay::instance = new MessageRelay();

    return instance;
}
