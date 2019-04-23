/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2016, 2015, 2014 Sebastian Wagner
 *
 * TraCurate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TraCurate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with TraCurate.  If not, see <https://www.gnu.org/licenses/>.
 */
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

namespace TraCurate { class Project; class Genealogy; }
std::ostream& operator<< (std::ostream&, TraCurate::Project&);

namespace TraCurate {
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
    struct XMLSliceSpec {
        QString tracks;
        QString xml;
        QList<QString> channels;
    };
    struct XMLProjectSpec {
        QString projectFile;
        qint32 rows;
        qint32 cols;
        QList<XMLSliceSpec> slices;
    };

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

    inline std::shared_ptr<CoordinateSystemInfo> getCoordinateSystemInfo() const { return coordinateSystemInfo; }
    void setCoordinateSystemInfo(const std::shared_ptr<CoordinateSystemInfo> &value);

    QString getFileName() const;
    void setFileName(const QString &value);

    XMLProjectSpec getProjectSpec() const;
    void setProjectSpec(const XMLProjectSpec &value);

    bool getImported() const;
    void setImported(bool value);

private:
    std::shared_ptr<Info> info; /*!< the Info-object for this Project */
    std::shared_ptr<Movie> movie; /*!< the Movie-object for this Project */
    QHash<int,std::shared_ptr<AutoTracklet>> autoTracklets; /*!< the AutoTracklet%s contained in this Project */
    std::shared_ptr<Genealogy> genealogy; /*!< the Genealogy for this Project */
    QList<std::string> inputFiles; /*!< currently unused */
    std::shared_ptr<CoordinateSystemInfo> coordinateSystemInfo; /*!< the CoordinateSystemInfo for this Project */
    QString fileName; /*!< the name of the file in which this project is stored */
    XMLProjectSpec projectSpec;
    bool imported;
};

}
#endif // PROJECT_H
