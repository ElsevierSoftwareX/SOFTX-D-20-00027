#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlComponent>

#if 0
#include "examples/examples.h"
#endif
#include "src/provider/imageprovider.h"
#include "src/provider/importobject.h"

#include <QFile>
#include <QTextStream>
#include <string>
#include <sstream>
#include <QDebug>
#include <QImageWriter>

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

  ImageProvider *provider = new ImageProvider();

  QQmlApplicationEngine engine;
  engine.addImageProvider("celltracking", provider);

  //QMetaType::registerType("Widget", 1, 0, "Widget");

  ImportObject MyImport;
  engine.rootContext()->setContextProperty("myImport", &MyImport);
  engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

  QObject *root = engine.rootObjects().first();
  QObject *area = root->findChild<QObject*>("mouseArea");
  provider->setMouseArea(area);
  MyImport.setProvider(provider);

  return app.exec();
}
