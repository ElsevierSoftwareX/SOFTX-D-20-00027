#include "examples.h"

#include <QDebug>
#include <H5Cpp.h>

#include "project.h"
#include "corrected_data/tracklet.h"
#include "export/exporthdf5.h"
#include "import/importhdf5.h"

void exampleWriteHDF5() {
    std::shared_ptr<CellTracker::Project> p;
    {
        CellTracker::ImportHDF5 imp;
        p = imp.load(QString(EXAMPLE_WRITE_HDF5));
    }

    std::shared_ptr<CellTracker::Tracklet> t1 = std::shared_ptr<CellTracker::Tracklet>(new CellTracker::Tracklet());
    std::shared_ptr<CellTracker::Tracklet> t2 = std::shared_ptr<CellTracker::Tracklet>(new CellTracker::Tracklet());
    std::shared_ptr<CellTracker::Tracklet> t3 = std::shared_ptr<CellTracker::Tracklet>(new CellTracker::Tracklet());

    std::shared_ptr<CellTracker::Frame> f1 = p->getMovie()->getFrame(0);
    std::shared_ptr<CellTracker::Frame> f2 = p->getMovie()->getFrame(1);
    std::shared_ptr<CellTracker::Frame> f3 = p->getMovie()->getFrame(2);

    std::shared_ptr<CellTracker::Object> o1 = f1->getSlice(0)->getChannel(0)->getObjects().values().at(0);
    std::shared_ptr<CellTracker::Object> o2 = f1->getSlice(0)->getChannel(0)->getObjects().values().at(1);
    std::shared_ptr<CellTracker::Object> o3 = f1->getSlice(0)->getChannel(0)->getObjects().values().at(2);
    std::shared_ptr<CellTracker::Object> o4 = f1->getSlice(0)->getChannel(0)->getObjects().values().at(3);
    std::shared_ptr<CellTracker::Object> o5 = f1->getSlice(0)->getChannel(0)->getObjects().values().at(4);

    t1->addToContained(f1,o1);
    t1->addToContained(f2,o3);
    t1->addToContained(f3,o5);

    t2->addToContained(f1,o2);
    t2->addToContained(f2,o4);
    t2->addToContained(f3,o3);

    t3->addToContained(f1,o3);
    t3->addToContained(f2,o1);
    t3->addToContained(f3,o1);

    p->getGenealogy()->addTracklet(t1);
    p->getGenealogy()->addTracklet(t2);
    p->getGenealogy()->addTracklet(t3);

    p->getGenealogy()->addAnnotation(t1, "Test TrackletAnnotation 1");
    p->getGenealogy()->addAnnotation(t2, "Test TrackletAnnotation 2");
    p->getGenealogy()->addAnnotation(t3, "Test TrackletAnnotation 3");

    p->getGenealogy()->addAnnotation(o1, "Test ObjectAnnotation 1");
    p->getGenealogy()->addAnnotation(o3, "Test ObjectAnnotation 2");
    p->getGenealogy()->addAnnotation(o5, "Test ObjectAnnotation 3");

    {
        CellTracker::ExportHDF5 exp;
        exp.save(p, QString(EXAMPLE_WRITE_HDF5));
    }

    return;
}

