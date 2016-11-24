#include "timetracker.h"
#include <chrono>
#include <QThread>

#include "guistate.h"
#include "io/hdf5_aux.h"

namespace CellTracker {

static std::chrono::steady_clock::duration poll_interval = std::chrono::seconds(1);

void TimeTracker::run() {
    using namespace std::chrono;
    GUIState *gs = GUIState::getInstance();

    while (true && !this->isInterruptionRequested()) {
        QString path = gs->getProjPath();
        if (path.compare("") != 0) {
            uint64_t last64 = gs->getWorkedOnProject();
            //    uint64_t last64 = 0;
            steady_clock::duration last = seconds(last64);
            steady_clock::duration next = last + poll_interval;

            std::this_thread::sleep_for(poll_interval);
            GUIState::getInstance()->setWorkedOnProject(duration_cast<seconds>(next).count());
        } else {
            this->msleep(100);
        }
    }
}

TimeTracker::TimeTracker() {
    start();
}

TimeTracker::~TimeTracker() {
    this->requestInterruption();
    this->wait();
}

}
