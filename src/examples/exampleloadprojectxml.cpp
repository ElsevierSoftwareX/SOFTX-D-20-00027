#include "examples.h"

#include <memory>
#include <sstream>

#include "import/importxml.h"

void exampleLoadProjectXML() {
    /* Load a movie */
    std::shared_ptr<CellTracker::Project> proj;
    CellTracker::ImportXML ix;
    proj = ix.load("/home/sebastian/arbeit/dev/data_old/14-04-10-Pos-13-after-well_6");

    /* Write the movie to a file */
    QFile file("/home/sebastian/celltest");
    std::ostringstream stream;
    stream << *proj;
    file.open(QIODevice::Truncate|QIODevice::Text|QIODevice::WriteOnly);
    file.write(stream.str().c_str());
    file.flush();
    file.close();
}
