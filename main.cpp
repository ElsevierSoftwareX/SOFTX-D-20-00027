#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlComponent>

#if 0
#include "src/base_data/movie.h"
#include "src/import/importxml.h"
#include "src/import/importhdf5.h"
#include "exceptions/ctimportexception.h"
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

#if 0
  ImportHDF5 ih;
  std::shared_ptr<QImage> img;
  for (int i =0; i<209; i++) {
      img = ih.requestImage("/home/sebastian/arbeit/dev/projects_celltracker2/big-example-data.h5", i, 0, 0);
//      img = ih.requestImage("/home/sebastian/arbeit/dev/projects_celltracker/data format/example/celltracker.h5", i, 0, 0);
      char *cs = new char[200];
      sprintf(cs,"%03d",i+1);
      QImageWriter writer((std::string("/tmp/own_image") + cs + ".png").c_str());
      writer.write(*img);
  }
#endif

#if 0
  std::shared_ptr<CellTracker::Project> proj;
  CellTracker::ImportHDF5 ih;

  try {
      proj = ih.load("/home/sebastian/arbeit/dev/projects_celltracker2/big-example-data.h5");
//  proj = ih.load("/home/sebastian/arbeit/dev/projects_celltracker2/big-example-data.h");
  } catch (CellTracker::CTException &e) {
      std::cerr << e.what();
  }

  return 0;
#endif

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

  ImageProvider *provider = new ImageProvider();

  QQmlApplicationEngine engine;
  engine.addImageProvider("celltracking", provider);
  engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

  //QMetaType::registerType("Widget", 1, 0, "Widget");

  ImportObject MyImport;
  engine.rootContext()->setContextProperty("myImport", &MyImport);

  QObject *root = engine.rootObjects().first();
  QObject *area = root->findChild<QObject*>("mouseArea");
  provider->setMouseArea(area);
  MyImport.setProvider(provider);

  return app.exec();
}
