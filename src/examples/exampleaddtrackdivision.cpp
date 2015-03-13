#include "examples.h"

#include <memory>

#include "import/importhdf5.h"
#include "corrected_data/trackletregular.h"

void exampleAddTrackDivision() {
    std::shared_ptr<CellTracker::Project> proj;
    CellTracker::ImportHDF5 ih;
    proj = ih.load(CURRENT_EXAMPLE);

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
}
