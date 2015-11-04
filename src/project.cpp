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
QHash<int,std::shared_ptr<AutoTracklet> > Project::getAutoTracklets() const
{
    return autoTracklets;
}

std::shared_ptr<AutoTracklet> Project::getAutoTracklet(int id) const
{
   return this->autoTracklets.value(id, nullptr);
}

void Project::addAutoTracklet(const std::shared_ptr<AutoTracklet> &autot)
{
    autoTracklets.insert(autot->getID(), autot);
}

std::shared_ptr<Project::CoordinateSystemInfo> Project::getCoordinateSystemInfo() const
{
    return coordinateSystemInfo;
}

void Project::setCoordinateSystemInfo(const std::shared_ptr<CoordinateSystemInfo> &value)
{
    coordinateSystemInfo = value;
}

QString Project::getFileName() const
{
    return fileName;
}

void Project::setFileName(const QString &value)
{
    fileName = value;
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
