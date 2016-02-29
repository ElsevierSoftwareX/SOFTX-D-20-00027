#include "examples.h"

#include <memory>

#include "exceptions/ctexception.h"
#include "io/importhdf5.h"

/*!
 * \brief demonstrates how to load a project from HDF5
 */
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
