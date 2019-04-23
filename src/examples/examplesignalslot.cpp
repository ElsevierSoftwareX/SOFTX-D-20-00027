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

        TraCurate::MessageRelay::emitUpdateOverallName("Importing something.");
        TraCurate::MessageRelay::emitUpdateOverallMax(5);

        for (int i=0; i < 5; i++) {
            std::string name = "Phase " + std::to_string(i);
            TraCurate::MessageRelay::emitUpdateDetailName(QString::fromStdString(name));
            TraCurate::MessageRelay::emitUpdateDetailMax(10);

            for (int j=0; j < 10; j++) {
                TraCurate::MessageRelay::emitIncreaseDetail();
                this->msleep(200);
            }

            TraCurate::MessageRelay::emitIncreaseOverall();
        }

        TraCurate::MessageRelay::emitFinishNotification();
    }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wexit-time-destructors"
#pragma clang diagnostic ignored "-Wglobal-constructors"
static ExampleThread eThread;
#pragma clang diagnostic pop

/*!
 * \brief demonstrates how to send messages via MessageRelay
 */
void exampleSignalSlot() {
    ::eThread.start();
}
