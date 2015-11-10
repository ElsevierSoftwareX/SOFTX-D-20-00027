#include "examples.h"

#include <memory>

#include "io/importhdf5.h"

void exampleAddTrackDivision() {
    std::shared_ptr<CellTracker::Project> proj;
    CellTracker::ImportHDF5 ih;
    proj = ih.load(CURRENT_EXAMPLE);

    std::shared_ptr<CellTracker::Genealogy> gen = proj->getGenealogy();

    std::shared_ptr<CellTracker::Tracklet> m = std::shared_ptr<CellTracker::Tracklet>(new CellTracker::Tracklet());
    std::shared_ptr<CellTracker::Tracklet> d1 = std::shared_ptr<CellTracker::Tracklet>(new CellTracker::Tracklet());
    std::shared_ptr<CellTracker::Tracklet> d2 = std::shared_ptr<CellTracker::Tracklet>(new CellTracker::Tracklet());
    std::shared_ptr<CellTracker::Tracklet> d3 = std::shared_ptr<CellTracker::Tracklet>(new CellTracker::Tracklet());

    int fId = 0;
    int mId = 3343, d1Id = 3344, d2Id = 3345, d3Id = 3346;
    int o1Id = 4711, o2Id = 4712, o3Id = 4713;

    std::shared_ptr<CellTracker::Frame> f = std::shared_ptr<CellTracker::Frame>(new CellTracker::Frame(fId));
    std::shared_ptr<CellTracker::Object> o1 = std::shared_ptr<CellTracker::Object>(new CellTracker::Object(o1Id, 0, 0, 0));
    std::shared_ptr<CellTracker::Object> o2 = std::shared_ptr<CellTracker::Object>(new CellTracker::Object(o2Id, 0, 0, 0));
    std::shared_ptr<CellTracker::Object> o3 = std::shared_ptr<CellTracker::Object>(new CellTracker::Object(o3Id, 0, 0, 0));

    d1->addToContained(f, o1);
    d1->addToContained(f, o2);
    d1->addToContained(f, o3);

    m->setId(mId);
    d1->setId(d1Id);
    d2->setId(d2Id);
    d3->setId(d3Id);

    gen->addTracklet(m);
    gen->addTracklet(d1);
    gen->addTracklet(d2);
    gen->addTracklet(d3);

    gen->addDaughterTrack(m,o1);
    gen->addDaughterTrack(m,o2);
    gen->addDaughterTrack(m,o3);

    std::cerr << *(gen->getTracklet(mId)) << std::endl;
}
