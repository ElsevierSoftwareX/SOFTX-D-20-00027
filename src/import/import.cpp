#include "import.h"

#include "project.h"
#include "base_data/info.h"
#include "base_data/movie.h"
#include "corrected_data/genealogy.h"

Import::Import()
{
}

std::shared_ptr<Project> Import::setupEmptyProject()
{
    std::shared_ptr<Info> info(new Info());
    std::shared_ptr<Project> project(new Project());
    std::shared_ptr<Movie> movie(new Movie());
    std::shared_ptr<Genealogy> genealogy(new Genealogy());

    project->setInfo(info);
    project->setMovie(movie);
    project->setGenealogy(genealogy);

    return project;
}
