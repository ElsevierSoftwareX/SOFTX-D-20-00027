#include "examples.h"

#include <memory>

#include "exceptions/ctexception.h"
#include "io/importhdf5.h"

void exampleAddAnnotation() {
    std::shared_ptr<CellTracker::Project> proj;
    CellTracker::ImportHDF5 ih;
    try {
        proj = ih.load(CURRENT_EXAMPLE);

        int nr = 0;
        std::shared_ptr<CellTracker::Genealogy> g = proj->getGenealogy();
        std::shared_ptr<CellTracker::Tracklet> t = g->getTracklet(nr);
        proj->getGenealogy()->addAnnotation(t,"Some annotation title","Some annotation description");

        std::shared_ptr<CellTracker::Annotation> a = proj->getGenealogy()->getAnnotation(t);
        std::cerr << *a;
    } catch (CellTracker::CTException &e) {
        std::cerr << e.what();
    }

}
