#include "project.h"

Project::Project()
{
}
std::shared_ptr<Movie> Project::getMovie() const
{
    return movie;
}

void Project::setMovie(const std::shared_ptr<Movie> &value)
{
    movie = value;
}
QList<std::shared_ptr<AutoTracklet> > Project::getAutoTracklets() const
{
    return autoTracklets;
}

void Project::addAutoTracklet(const std::shared_ptr<AutoTracklet> &value)
{
    autoTracklets.append(value);
}
std::shared_ptr<Genealogy> Project::getGenealogy() const
{
    return genealogy;
}

void Project::setGenealogy(const std::shared_ptr<Genealogy> &value)
{
    genealogy = value;
}





std::ostream &operator<<(std::ostream &strm, Project &p)
{
    strm << "Project:\n";
    strm << "movie:\n";
    strm << *(p.movie);
    strm << "autoTracklets:\n";
    for (std::shared_ptr<AutoTracklet> at: p.autoTracklets){
        strm << *at;
    }
    strm << "genealogy:\n";
    strm << *(p.genealogy);
    return strm;
}
