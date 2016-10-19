#include "examples.h"

#include "project.h"
#include "io/importhdf5.h"

#define REPS 1

void exampleloadmultipletimes() {
    using namespace CellTracker;

    ImportHDF5 ih;
    std::shared_ptr<Project> proj;

    for (int i = 0; i < REPS; i++) {
        proj = ih.load(EXAMPLE_SMALL_HDF5);
    }
    exit(0);
}
