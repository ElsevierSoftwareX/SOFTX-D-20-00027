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
/*!
 * \brief The Project class
 *
 * This class has (direct or indirect) references to almost any runtime data.
 * This includes the Movie, Genealogy and AutoTracklet%s
 *
 * It also saves, which coordinate system is used in an instance of CoordinateSystemInfo.
 */
class Project
{
    friend std::ostream& ::operator<< (std::ostream&, Project&);

public:
    Project() = default;

    /*!
     * \brief The CoordinateSystemInfo class
     *
     * An inner class to represent, which coordinate system is currently used
     */
    class CoordinateSystemInfo {
    public:
        /*!
         * \brief The CoordinateSystemType enum
         *
         * Currently either CST_CARTESIAN or CST_QTIMAGE
         */
        enum CoordinateSystemType {
            CST_CARTESIAN,      /*!< has its origin on the bottom left */
            CST_QTIMAGE         /*!< has its origin on the top left */
        };
        /*!
         * \brief The CoordinateSystemData struct
         *
         * Specifies image height and width.
         */
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
        CoordinateSystemType cst; /*!< the coordinate system type */
        CoordinateSystemData csd; /*!< the coordinate system info */


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

    QString getFileName() const;
    void setFileName(const QString &value);

private:
    std::shared_ptr<Info> info; /*!< the Info-object for this Project */
    std::shared_ptr<Movie> movie; /*!< the Movie-object for this Project */
    QHash<int,std::shared_ptr<AutoTracklet>> autoTracklets; /*!< the AutoTracklet%s contained in this Project */
    std::shared_ptr<Genealogy> genealogy; /*!< the Genealogy for this Project */
    QList<std::string> inputFiles; /*!< currently unused */
    std::shared_ptr<CoordinateSystemInfo> coordinateSystemInfo; /*!< the CoordinateSystemInfo for this Project */
    QString fileName; /*!< the name of the file in which this project is stored */
};

}
#endif // PROJECT_H
