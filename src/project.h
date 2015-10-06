#ifndef PROJECT_H
#define PROJECT_H

#include <iostream>
#include <memory>
#include <string>

#include <QHash>

#include "base/info.h"
#include "base/movie.h"
#include "base/autotracklet.h"
#include "tracked/genealogy.h"

namespace CellTracker { class Project; class Genealogy; }
std::ostream& operator<< (std::ostream&, CellTracker::Project&);

namespace CellTracker {

class Project
{
    friend std::ostream& ::operator<< (std::ostream&, Project&);

public:
    Project();

    class CoordinateSystemInfo {
    public:
        enum CoordinateSystemType {
            CST_CARTESIAN,      /* origin on the bottom left */
            CST_QTIMAGE         /* origin on the top left */
        };
        struct CoordinateSystemData {
            uint32_t imageHeight;
            uint32_t imageWidth;
        };

        CoordinateSystemInfo() {}
        CoordinateSystemType getCoordinateSystemType() const { return cst; }
        void setCoordinateSystemType(const CoordinateSystemType &value) { cst = value; }
        CoordinateSystemData getCoordinateSystemData() const { return csd; }
        void setCoordinateSystemData(const CoordinateSystemData &value) { csd = value; }
    private:
        CoordinateSystemType cst;
        CoordinateSystemData csd;


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

    std::shared_ptr<CoordinateSystemInfo> getCoordinateSystemInfo() const;
    void setCoordinateSystemInfo(const std::shared_ptr<CoordinateSystemInfo> &value);

private:
    std::shared_ptr<Info> info;
    std::shared_ptr<Movie> movie;
    QHash<int,std::shared_ptr<AutoTracklet>> autoTracklets;
    std::shared_ptr<Genealogy> genealogy;
    QList<std::string> inputFiles;
    std::shared_ptr<CoordinateSystemInfo> coordinateSystemInfo;
};

}
#endif // PROJECT_H
