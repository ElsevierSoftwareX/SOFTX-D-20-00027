#ifndef PROJECT_H
#define PROJECT_H

#include <iostream>
#include <memory>
#include <string>

#include <QHash>

#include "base_data/info.h"
#include "base_data/movie.h"
#include "base_data/autotracklet.h"
#include "corrected_data/genealogy.h"

namespace CellTracker { class Project; class Genealogy; }
std::ostream& operator<< (std::ostream&, CellTracker::Project&);

namespace CellTracker {

class Project
{
    friend std::ostream& ::operator<< (std::ostream&, Project&);

public:
    Project();

    enum CoordinateFormat {
        CF_CARTESIAN
    };

    std::shared_ptr<Info> getInfo() const;
    void setInfo(const std::shared_ptr<Info> &value);

    std::shared_ptr<Movie> getMovie() const;
    void setMovie(const std::shared_ptr<Movie> &value);

    std::shared_ptr<Genealogy> getGenealogy() const;
    void setGenealogy(const std::shared_ptr<Genealogy> &value);

    QHash<int, std::shared_ptr<AutoTracklet> > getAutoTracklets() const;
    std::shared_ptr<AutoTracklet> getAutoTracklet(int) const;
    void addAutoTracklet(const std::shared_ptr<AutoTracklet> &value);

    CoordinateFormat getCoordinateFormat() const;
    void setCoordinateFormat(const CoordinateFormat &value);

private:
    std::shared_ptr<Info> info;
    std::shared_ptr<Movie> movie;
    QHash<int,std::shared_ptr<AutoTracklet>> autoTracklets;
    std::shared_ptr<Genealogy> genealogy;
    QList<std::string> inputFiles;
    CoordinateFormat coordinateFormat;
};

}
#endif // PROJECT_H
