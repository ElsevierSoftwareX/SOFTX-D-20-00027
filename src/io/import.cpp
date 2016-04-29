#include "import.h"

#include "project.h"
#include "base/info.h"
#include "base/movie.h"
#include "tracked/genealogy.h"

namespace CellTracker {

Import::~Import() {}

/*!
 * \brief sets up an empty Project and instantiates all required Objects (Info,
 * Movie, Genealogy) to work on it.
 *
 * \return the generated Project
 */
std::shared_ptr<Project> Import::setupEmptyProject()
{
    auto info      = std::make_shared<Info>();
    auto project   = std::make_shared<Project>();
    auto movie     = std::make_shared<Movie>();
    auto genealogy = std::make_shared<Genealogy>(project);

    project->setInfo(info);
    project->setMovie(movie);
    project->setGenealogy(genealogy);

    return project;
}

}
