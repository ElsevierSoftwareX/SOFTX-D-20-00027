#include "examples.h"
#include "provider/messagerelay.h"

#include <QThread>
#include <QDebug>

class ExampleThread : public QThread {
public:
    void run() {
        qDebug() << "sleeping";
        this->sleep(5);
        qDebug() << "slept";

        CellTracker::MessageRelay::emitUpdateOverallName("Importing something.");
        CellTracker::MessageRelay::emitUpdateOverallMax(5);

        for (int i=0; i < 5; i++) {
            std::string name = "Phase " + std::to_string(i);
            CellTracker::MessageRelay::emitUpdateDetailName(QString::fromStdString(name));
            CellTracker::MessageRelay::emitUpdateDetailMax(10);

            for (int j=0; j < 10; j++) {
                CellTracker::MessageRelay::emitIncreaseDetail();
                this->msleep(200);
            }

            CellTracker::MessageRelay::emitIncreaseOverall();
        }

        CellTracker::MessageRelay::emitFinishNotification();
    }
};

ExampleThread eThread;

void exampleSignalSlot() {
    ::eThread.start();
}
