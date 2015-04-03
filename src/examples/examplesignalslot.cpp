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

        MessageRelay::emitUpdateOverallName("Importing something.");
        MessageRelay::emitUpdateOverallMax(5);

        for (int i=0; i < 5; i++) {
            std::string name = "Phase " + std::to_string(i);
            MessageRelay::emitUpdateDetailName(QString::fromStdString(name));
            MessageRelay::emitUpdateDetailMax(10);

            for (int j=0; j < 10; j++) {
                MessageRelay::emitIncreaseDetail();
                this->msleep(200);
            }

            MessageRelay::emitIncreaseOverall();
        }

        MessageRelay::emitFinishNotification();
    }
};

ExampleThread eThread;

void exampleSignalSlot() {
    ::eThread.start();
}
