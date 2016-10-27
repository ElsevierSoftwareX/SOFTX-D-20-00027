#include "project.h"

namespace CellTracker {

/*!
 * \brief returns the movie
 * \return the movie
 */
std::shared_ptr<Movie> Project::getMovie() const
{
    return movie;
}

/*!
 * \brief sets the movie
 * \param value the movie to set
 */
void Project::setMovie(const std::shared_ptr<Movie> &value)
{
    movie = value;
}

/*!
 * \brief returns all AutoTracklet%s
 * \return all AutoTracklet%s
 */
QHash<int,std::shared_ptr<AutoTracklet> > Project::getAutoTracklets() const
{
    return autoTracklets;
}

/*!
 * \brief returns a specific AutoTracklet%s
 * \param id the ID of the AutoTracklet to return
 * \return the requested AutoTracklet or nullptr if no such AutoTracklet exists
 */
std::shared_ptr<AutoTracklet> Project::getAutoTracklet(int id) const
{
   return this->autoTracklets.value(id, nullptr);
}

/*!
 * \brief adds a AutoTracklet to the Project
 * \param autot the AutoTracklet to add
 */
void Project::addAutoTracklet(const std::shared_ptr<AutoTracklet> &autot)
{
    autoTracklets.insert(autot->getID(), autot);
}

/*!
 * \brief sets the Project::CoordinateSystemInfo
 * \param value the Project::CoordinateSystemInfo to set to
 */
void Project::setCoordinateSystemInfo(const std::shared_ptr<CoordinateSystemInfo> &value)
{
    coordinateSystemInfo = value;
}

/*!
 * \brief returns the filename that is associated with this Project
 * \return the filename
 */
QString Project::getFileName() const
{
    return fileName;
}

/*!
 * \brief sets the filename of this Project
 * \param value the filename that should be set
 */
void Project::setFileName(const QString &value)
{
    fileName = value;
}

/*!
 * \brief returns thet current Info object
 * \return the Info object
 * \warning the Info object is currently unused
 */
std::shared_ptr<Info> Project::getInfo() const
{
    return info;
}

/*!
 * \brief sets the current Info object
 * \param value the Info object that should be set
 * \warning the Info object is currently unused
 */
void Project::setInfo(const std::shared_ptr<Info> &value)
{
    info = value;
}

/*!
 * \brief returns the Genealogy of the Project
 * \return the Genealogy of the Project
 */
std::shared_ptr<Genealogy> Project::getGenealogy() const
{
    return genealogy;
}

/*!
 * \brief sets the Genealogy of this Project
 * \param value the new Genealogy
 */
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
