#include "examples.h"

#include <iostream>

#include "project.h"
#include "io/importhdf5.h"

/*!
 * \brief demonstrates how to write a project to stdout
 * \warning most likely doesn't work anymore
 */
void exampleWriteProjectStdout() {
    CellTracker::ImportHDF5 ih;
    std::shared_ptr<CellTracker::Project> p = ih.load(EXAMPLE_SMALL_FILE);

    std::cout << *p;
}
