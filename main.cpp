#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlComponent>

#if 0
#include "examples/examples.h"
#endif
#include "provider/ctsettings.h"
#include "provider/dataprovider.h"
#include "provider/guicontroller.h"
#include "provider/guistate.h"
#include "provider/imageprovider.h"
#include "provider/messagerelay.h"

#include <QFile>
#include <QTextStream>
#include <string>
#include <sstream>
#include <QDebug>
#include <QtQml>
#include <QImageWriter>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    CellTracker::CTSettings::getInstance();
    CellTracker::GUIState::getInstance();
    CellTracker::DataProvider::getInstance();

    CellTracker::ImageProvider *provider = new CellTracker::ImageProvider();

    qmlRegisterSingletonType<CellTracker::CTSettings>   ("imb.celltracker", 1,0, "CTSettings",    CellTracker::CTSettings::qmlInstanceProvider);
    qmlRegisterSingletonType<CellTracker::GUIController>("imb.celltracker", 1,0, "GUIController", CellTracker::GUIController::qmlInstanceProvider);
    qmlRegisterSingletonType<CellTracker::GUIState>     ("imb.celltracker", 1,0, "GUIState",      CellTracker::GUIState::qmlInstanceProvider);
    qmlRegisterSingletonType<CellTracker::DataProvider> ("imb.celltracker", 1,0, "DataProvider",  CellTracker::DataProvider::qmlInstanceProvider);
    qmlRegisterSingletonType<CellTracker::MessageRelay> ("imb.celltracker", 1,0, "MessageRelay",  CellTracker::MessageRelay::qmlInstanceProvider);
    qmlRegisterType<CellTracker::Annotation> ("imb.celltracker", 1,0, "Annotation");
    qmlRegisterType<CellTracker::Tracklet>   ("imb.celltracker", 1,0, "Tracklet");

    engine.addImageProvider("celltracking", provider);
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    int ret = app.exec();

    return ret;
}
