#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlComponent>

#if 0
#include "examples/examples.h"
#endif
#include "src/provider/ctsettings.h"
#include "src/provider/imageprovider.h"
#include "src/provider/imageprovider2.h"
#include "src/provider/dataprovider.h"
#include "provider/guistate.h"
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

  CellTracker::CTSettings::getInstance();
  CellTracker::GUIState::getInstance();
  CellTracker::DataProvider::getInstance();

  ImageProvider2 *provider2 = new ImageProvider2();

  qmlRegisterSingletonType<CellTracker::CTSettings>("imb.celltracker.settings", 1,0,"CTSettings", CellTracker::CTSettings::qmlInstanceProvider);
  qmlRegisterSingletonType<CellTracker::GUIState>("imb.celltracker.guistate", 1,0,"GUIState", CellTracker::GUIState::qmlInstanceProvider);
  qmlRegisterSingletonType<CellTracker::DataProvider>("imb.celltracker.data", 1,0,"DataProvider", CellTracker::DataProvider::qmlInstanceProvider);
  qmlRegisterSingletonType<MessageRelay>("imb.celltracker.messagerelay", 1, 0, "MessageRelay", MessageRelay::qmlInstanceProvider);

  QQmlApplicationEngine engine;
  engine.addImageProvider("celltracking2", provider2);

//  engine.rootContext()->setContextProperty("messageRelay", MessageRelay::getInstance());
  engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

  QObject *root = engine.rootObjects().first();

  int ret = app.exec();

  return ret;
}
