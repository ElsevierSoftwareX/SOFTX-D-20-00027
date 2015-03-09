#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlComponent>

#if 0
#include "src/project.h"
#include "src/base_data/movie.h"
#include "src/corrected_data/trackletregular.h"
#include "src/corrected_data/trackeventdivision.h"
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
  CellTracker::ImportHDF5 ih;
  std::shared_ptr<QImage> img;
  for (int i =0; i<209; i++) {
//      img = ih.requestImage("/home/sebastian/arbeit/dev/smaller-example-data.h5", i, 0, 0);
      img = ih.requestImage("/home/sebastian/arbeit/dev/big-example-data.h5", i, 0, 0);
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
      proj = ih.load("/home/sebastian/arbeit/dev/smaller-example-data.h5");
//      proj = ih.load("/home/sebastian/arbeit/dev/big-example-data.h5");

      std::cerr << *proj;
  } catch (CellTracker::CTException &e) {
      std::cerr << e.what();
  }

  return 0;
#endif

#if 0
  CellTracker::ImportHDF5 ih;
  std::shared_ptr<CellTracker::Project> proj;
  std::shared_ptr<QImage> img;
  try {
      QString in_file = "/home/sebastian/arbeit/dev/smaller-example-data.h5";
      QString tmp_file = "/tmp/image.png";

      proj = ih.load(in_file);
      img = ih.requestImage(in_file, 0, 0, 0);
      QImageWriter writer (tmp_file);
      writer.write(*img);

      std::cout << "set terminal pngcairo transparent" << std::endl
                << "set output 'out.png'" << std::endl
                << "set multiplot" << std::endl
                << "set size ratio 1" << std::endl
                << "set autoscale fix" << std::endl
                << "plot '" << tmp_file.toStdString() << "' binary filetype=png with rgbimage" << std::endl;
      for (std::shared_ptr<CellTracker::Object> o: proj->getMovie()->getFrame(0)->getSlice(0)->getObjects().values()) {
          std::shared_ptr<QPolygonF> poly = o->getOutline();
          std::shared_ptr<QRect> rect = o->getBoundingBox();

          int length = poly->size();

          std::cout << "set object polygon from ";
          for (int i = 0; i<length; i++) {
              QPointF p = poly->at(i);
              int x = rect->topLeft().x() - rect->topLeft().y() + p.y();
              int y = rect->topLeft().y() - rect->topLeft().x() + p.x();
              if (i < length-1)
                  std::cout << x << "," << y << " to ";
              else
                  std::cout << x << "," << y << std::endl;
          }
      }
      std::cout << "set yrange [250:0]" << std::endl
                << "plot [0:250] [0:250] x" << std::endl
                << "unset multiplot" << std::endl
                << "set output" << std::endl;

      return 0;
  } catch (CellTracker::CTException &e) {
    std::cout << e.what();
  }
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

#if 0
  std::shared_ptr<CellTracker::Project> proj;
  CellTracker::ImportHDF5 ih;
  proj = ih.load("/home/sebastian/arbeit/dev/smaller-example-data.h5");

  std::shared_ptr<CellTracker::Genealogy> gen = proj->getGenealogy();

  std::shared_ptr<CellTracker::AutoTracklet> bt = std::shared_ptr<CellTracker::AutoTracklet>(new CellTracker::AutoTracklet());

  std::shared_ptr<CellTracker::Tracklet> m = std::shared_ptr<CellTracker::Tracklet>(new CellTracker::TrackletRegular(bt));
  std::shared_ptr<CellTracker::Tracklet> d1 = std::shared_ptr<CellTracker::Tracklet>(new CellTracker::TrackletRegular(bt));
  std::shared_ptr<CellTracker::Tracklet> d2 = std::shared_ptr<CellTracker::Tracklet>(new CellTracker::TrackletRegular(bt));
  std::shared_ptr<CellTracker::Tracklet> d3 = std::shared_ptr<CellTracker::Tracklet>(new CellTracker::TrackletRegular(bt));

  int mId = 3343, d1Id = 3344, d2Id = 3345, d3Id = 3346;

  m->setID(mId);
  d1->setID(d1Id);
  d2->setID(d2Id);
  d3->setID(d3Id);

  gen->addTracklet(m);
  gen->addTracklet(d1);
  gen->addTracklet(d2);
  gen->addTracklet(d3);

  gen->addDivision(mId);
  gen->addDaughterTrack(mId,d1Id);
  gen->addDaughterTrack(mId,d2Id);
  gen->addDaughterTrack(mId,d3Id);

  std::cerr << *(gen->getTracklet(mId)) << std::endl;
  return 0;
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
