#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "src/base_data/movie.h"
#include "src/import/importxml.h"

#include <QFile>
#include <QTextStream>
#include <string>
#include <sstream>
#include <QDebug>

int main(int argc, char *argv[])
{
  QGuiApplication app(argc, argv);

#if 0
  /* Load a movie */
  std::shared_ptr<Project> proj;
  ImportXML ix;
  proj = ix.load("/home/sebastian/arbeit/dev/data_old/14-04-10-Pos-13-after-well_6");

  /* Write the movie to a file */
  QFile file("/home/sebastian/celltest");
  std::ostringstream stream;
  stream << *proj;
  file.open(QIODevice::Truncate|QIODevice::Text|QIODevice::WriteOnly);
  file.write(stream.str().c_str());
  file.flush();
  file.close();
#endif

  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

  return app.exec();
}
