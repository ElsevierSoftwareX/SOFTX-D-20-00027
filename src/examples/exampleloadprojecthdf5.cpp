#include "examples.h"

#include <memory>

#include "exceptions/ctexception.h"
#include "import/importhdf5.h"

void exampleLoadProjectHDF5 () {
    std::shared_ptr<CellTracker::Project> proj;
    CellTracker::ImportHDF5 ih;
    try {
        proj = ih.load("/home/sebastian/arbeit/dev/smaller-example-data.h5");
//      proj = ih.load("/home/sebastian/arbeit/dev/big-example-data.h5");

        std::cerr << *proj;
    } catch (CellTracker::CTException &e) {
        std::cerr << e.what();
    }
}
