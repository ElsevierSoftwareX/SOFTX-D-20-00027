#include "project.h"

namespace CellTracker {

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

std::shared_ptr<Info> Project::getInfo() const
{
    return info;
}

void Project::setInfo(const std::shared_ptr<Info> &value)
{
    info = value;
}

std::shared_ptr<Genealogy> Project::getGenealogy() const
{
    return genealogy;
}

void Project::setGenealogy(const std::shared_ptr<Genealogy> &value)
{
    genealogy = value;
}

}

std::ostream &operator<<(std::ostream &strm, CellTracker::Project &p)
{
    strm << "Project:" << std::endl;
    strm << "info:" << std::endl;
    strm << *(p.info);
    strm << "movie:" << std::endl;
    strm << *(p.movie);
    strm << "autoTracklets:" << std::endl;
    for (std::shared_ptr<CellTracker::AutoTracklet> at: p.autoTracklets){
        strm << *at;
    }
    strm << "genealogy:" << std::endl;
    strm << *(p.genealogy);
    return strm;
}
