#include "examples.h"

#include <memory>
#include <sstream>

#include "io/importxml.h"

/*!
 * \brief demonstrates loading a project from XML
 * \warning most likely broken, as XML is no more supported
 */
void exampleLoadProjectXML() {
    /* Load a movie */
    std::shared_ptr<CellTracker::Project> proj;
    CellTracker::ImportXML ix;
    proj = ix.load(EXAMPLE_XML_FILE);

    /* Write the movie to a file */
    QFile file("/home/sebastian/celltest");
    std::ostringstream stream;
    stream << *proj;
    file.open(QIODevice::Truncate|QIODevice::Text|QIODevice::WriteOnly);
    file.write(stream.str().c_str());
    file.flush();
    file.close();
}
