#include "examples.h"

#include <memory>

#include "io/importhdf5.h"

/*!
 * \brief demonstrates how to add a TrackEventDivision to a Tracklet
 */
void exampleAddTrackDivision() {
    std::shared_ptr<CellTracker::Project> proj;
    CellTracker::ImportHDF5 ih;
    proj = ih.load(CURRENT_EXAMPLE);

    std::shared_ptr<CellTracker::Genealogy> gen = proj->getGenealogy();

    auto m = std::make_shared<CellTracker::Tracklet>();
    auto d1 = std::make_shared<CellTracker::Tracklet>();
    auto d2 = std::make_shared<CellTracker::Tracklet>();
    auto d3 = std::make_shared<CellTracker::Tracklet>();

    int fId = 0;
    int mId = 3343, d1Id = 3344, d2Id = 3345, d3Id = 3346;
    int o1Id = 4711, o2Id = 4712, o3Id = 4713;

    auto f = std::make_shared<CellTracker::Frame>(fId);
    auto o1 = std::make_shared<CellTracker::Object>(o1Id, 0, 0, 0);
    auto o2 = std::make_shared<CellTracker::Object>(o2Id, 0, 0, 0);
    auto o3 = std::make_shared<CellTracker::Object>(o3Id, 0, 0, 0);

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
