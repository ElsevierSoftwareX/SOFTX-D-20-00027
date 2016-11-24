#ifndef TIMETRACKER_H
#define TIMETRACKER_H

#include <thread>
#include <QThread>

namespace CellTracker {
class TimeTracker : public QThread
{
public:
    TimeTracker();
    ~TimeTracker();
    void run();
};
}

#endif // TIMETRACKER_H
