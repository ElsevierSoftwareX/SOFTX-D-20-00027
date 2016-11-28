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

protected:
    bool eventFilter(QObject *watched, QEvent *event);
};
}

#endif // TIMETRACKER_H
