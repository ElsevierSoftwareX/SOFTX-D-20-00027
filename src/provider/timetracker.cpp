/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2017, 2016 Sebastian Wagner
 *
 * Celltracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Celltracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Celltracker.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "timetracker.h"

#include <chrono>
#include <QApplication>
#include <QInputEvent>
#include <QThread>

#include "guistate.h"
#include "io/hdf5_aux.h"
#include "provider/ctsettings.h"

namespace CellTracker {

static std::chrono::steady_clock::duration poll_interval = std::chrono::seconds(1);
static std::chrono::steady_clock::duration timeout_interval = std::chrono::seconds(60);

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wglobal-constructors"
static std::chrono::steady_clock::time_point last_event = std::chrono::steady_clock::now();
#pragma clang diagnostic pop

static bool eligibleForAccounting() {
    /* find, if window is active */
    if (!QGuiApplication::focusWindow())
        return false;

    /* don't update if the last inputEvent was too long ago */
    if (std::chrono::steady_clock::now() > last_event + timeout_interval)
        return false;

    return true;
}

bool TimeTracker::eventFilter(QObject *watched, QEvent *event) {
    if (dynamic_cast<QInputEvent*>(event))
        last_event = std::chrono::steady_clock::now();

    return QObject::eventFilter(watched, event);
}

void TimeTracker::run() {
    using namespace std::chrono;
    GUIState *gs = GUIState::getInstance();

    while (!this->isInterruptionRequested()) {
        QString date = QDate::currentDate().toString(Qt::ISODate);
        QString path = gs->getProjPath();
        if (path.compare("") != 0 && CTSettings::value("time_tracking/track").toBool()) {
            QVariantMap wop = gs->getWorkedOnProject();

            if (!wop[date].isValid())
                wop[date] = QVariant::fromValue<qulonglong>(0);

            uint64_t last64 = wop[date].toULongLong();

            steady_clock::duration last = seconds(last64);
            steady_clock::duration next = last + poll_interval;

#ifdef __MINGW32__
	    this->msleep(1000);
#else
            std::this_thread::sleep_for(poll_interval);
#endif

            if (eligibleForAccounting())
                wop[date] = QVariant::fromValue<qulonglong>(duration_cast<seconds>(next).count());

            gs->setWorkedOnProject(wop);
        } else {
            this->msleep(100);
        }
    }
}

TimeTracker::TimeTracker() {
    QGuiApplication::instance()->installEventFilter(this);
    start();
}

TimeTracker::~TimeTracker() {
    this->requestInterruption();
    this->wait();
}

}
