#include "import.h"

#include "project.h"
#include "base/info.h"
#include "base/movie.h"
#include "tracked/genealogy.h"

namespace CellTracker {

Import::Import() { }
Import::~Import() { }

std::shared_ptr<Project> Import::setupEmptyProject()
{
    std::shared_ptr<Info> info(new Info());
    std::shared_ptr<Project> project(new Project());
    std::shared_ptr<Movie> movie(new Movie());
    std::shared_ptr<Genealogy> genealogy(new Genealogy(project));

    project->setInfo(info);
    project->setMovie(movie);
    project->setGenealogy(genealogy);

    return project;
}

}
