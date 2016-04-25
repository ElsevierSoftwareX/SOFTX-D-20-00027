#include "examples.h"

#include <QDebug>
#include <H5Cpp.h>

#include "project.h"
#include "tracked/annotation.h"
#include "tracked/tracklet.h"
#include "io/exporthdf5.h"
#include "io/importhdf5.h"

/*!
 * \brief demonstrates how to read a project, add some Tracklets to it, add
 * some Annotations, annotate some Objects and Tracklets and then write it out
 * to HDF5 again
 */
void exampleWriteHDF5() {
    std::shared_ptr<CellTracker::Project> p;
    {
        CellTracker::ImportHDF5 imp;
        p = imp.load(QString(EXAMPLE_WRITE_HDF5));
    }

    auto t1 = std::make_shared<CellTracker::Tracklet>();
    auto t2 = std::make_shared<CellTracker::Tracklet>();
    auto t3 = std::make_shared<CellTracker::Tracklet>();

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

    auto ta1 = std::make_shared<CellTracker::Annotation>(CellTracker::Annotation::TRACKLET_ANNOTATION, "Test Tracklet Title 1", "Test Tracklet Description 1");
    auto ta2 = std::make_shared<CellTracker::Annotation>(CellTracker::Annotation::TRACKLET_ANNOTATION, "Test Tracklet Title 2", "Test Tracklet Description 2");
    auto ta3 = std::make_shared<CellTracker::Annotation>(CellTracker::Annotation::TRACKLET_ANNOTATION, "Test Tracklet Title 3", "Test Tracklet Description 3");
    auto oa1 = std::make_shared<CellTracker::Annotation>(CellTracker::Annotation::OBJECT_ANNOTATION, "Test Object Title 1", "Test Object Description 1");
    auto oa2 = std::make_shared<CellTracker::Annotation>(CellTracker::Annotation::OBJECT_ANNOTATION, "Test Object Title 2", "Test Object Description 2");
    auto oa3 = std::make_shared<CellTracker::Annotation>(CellTracker::Annotation::OBJECT_ANNOTATION, "Test Object Title 3", "Test Object Description 3");

    p->getGenealogy()->addAnnotation(ta1);
    p->getGenealogy()->addAnnotation(ta2);
    p->getGenealogy()->addAnnotation(ta3);
    p->getGenealogy()->addAnnotation(oa1);
    p->getGenealogy()->addAnnotation(oa2);
    p->getGenealogy()->addAnnotation(oa3);

    p->getGenealogy()->annotate(t1, ta1);
    p->getGenealogy()->annotate(t2, ta2);
    p->getGenealogy()->annotate(t3, ta3);
    p->getGenealogy()->annotate(o1, oa1);
    p->getGenealogy()->annotate(o3, oa2);
    p->getGenealogy()->annotate(o5, oa3);

    {
        CellTracker::ExportHDF5 exp;
        exp.save(p, QString(EXAMPLE_WRITE_HDF5));
    }

    return;
}

