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
#include "messagerelay.h"

namespace TraCurate {
MessageRelay *MessageRelay::instance = nullptr;

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
