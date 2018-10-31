/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2016, 2015 Sebastian Wagner
 *
 * Celltracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Celltracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Celltracker.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef INFO_H
#define INFO_H

#include <iostream>
#include <string>

#include <QList>
#include <QDateTime>

namespace CellTracker { class Info; }
std::ostream& operator<<(std::ostream &, const CellTracker::Info &);

namespace CellTracker {

/*!
 * \brief The Info class
 *
 * This Class represents general information regarding the project.
 *
 * \warning This is currently unused and based on an older version of the DataFormat.
 *
 */
class Info
{
public:
    Info() = default;

    QList<std::string> getInputFiles() const __attribute__((deprecated));
    void setInputFiles(const QList<std::string> &value) __attribute__((deprecated));

    QDateTime getTimeOfConversion() const __attribute__((deprecated));
    void setTimeOfConversion(const QDateTime &value) __attribute__((deprecated));

    std::string getTrackingInfoAlgorithm() const __attribute__((deprecated));
    void setTrackingInfoAlgorithm(const std::string &value) __attribute__((deprecated));

    std::string getTrackingInfoILastikVersion() const __attribute__((deprecated));
    void setTrackingInfoILastikVersion(const std::string &value) __attribute__((deprecated));

    QDateTime getTrackingInfoTimeOfTracking() const __attribute__((deprecated));
    void setTrackingInfoTimeOfTracking(const QDateTime &value) __attribute__((deprecated));

    friend std::ostream& ::operator<<(std::ostream&, const Info&);
private:
    QList<std::string> inputFiles;
    QDateTime timeOfConversion;
    std::string trackingInfoAlgorithm;
    std::string trackingInfoILastikVersion;
    QDateTime trackingInfoTimeOfTracking;
};

}

#endif // INFO_H
