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
static std::chrono::steady_clock::time_point last_event = std::chrono::steady_clock::now();

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

            std::this_thread::sleep_for(poll_interval);

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
