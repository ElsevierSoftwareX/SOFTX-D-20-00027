#include "examples.h"

#include <memory>

#include "exceptions/ctexception.h"
#include "import/importhdf5.h"

void exampleLoadProjectHDF5 () {
    std::shared_ptr<CellTracker::Project> proj;
    CellTracker::ImportHDF5 ih;
    try {
        proj = ih.load(CURRENT_EXAMPLE);

        std::cerr << *proj;
    } catch (CellTracker::CTException &e) {
        std::cerr << e.what();
    }
}
