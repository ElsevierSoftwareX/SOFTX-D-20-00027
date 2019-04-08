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
#ifndef MESSAGERELAY_H
#define MESSAGERELAY_H

#include <QObject>
#include <QQmlEngine>
#include <QJSEngine>

namespace TraCurate {
/*!
 * \brief The MessageRelay class
 *
 * The MessageRelay is used to relay Messages from C++-Classes to QML-Code without
 * them needing to implement QObject.
 */
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

    /*!
     * \brief updates the overall name in the status window
     * \param text the new overall name
     */
    static void emitUpdateOverallName(QString text) { MessageRelay::getInstance()->updateOverallName(text); }
    /*!
     * \brief updates the overall maximum value
     * \param newMax the new overall maximum
     */
    static void emitUpdateOverallMax(int newMax) { MessageRelay::getInstance()->updateOverallMax(newMax); }
    /*!
     * \brief increases the overall counter
     */
    static void emitIncreaseOverall() { MessageRelay::getInstance()->increaseOverall(); }

    /*!
     * \brief updates the detailed name in the status window
     * \param text the new detailed name
     */
    static void emitUpdateDetailName(QString text) { MessageRelay::getInstance()->updateDetailName(text); }
    /*!
     * \brief updates the detailed maximum value
     * \param newMax the new detailed maximum
     */
    static void emitUpdateDetailMax(int newMax) { MessageRelay::getInstance()->updateDetailMax(newMax); }
    /*!
     * \brief increases the detailed counter
     */
    static void emitIncreaseDetail() { MessageRelay::getInstance()->increaseDetail(); }

    /*!
     * \brief emits a finish notification
     */
    static void emitFinishNotification() { MessageRelay::getInstance()->finishNotification(); }

    /*!
     * \brief updates the statusbar with a message
     * \param message the message to display
     */
    static void emitUpdateStatusBar(QString message) { MessageRelay::getInstance()->updateStatusBar(message); }

private:
    MessageRelay() = default;

    static MessageRelay *instance;
};
}

#endif // MESSAGERELAY_H
