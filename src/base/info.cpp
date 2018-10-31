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
#include "info.h"

namespace CellTracker {

QList<std::string> Info::getInputFiles() const
{
    return inputFiles;
}

void Info::setInputFiles(const QList<std::string> &value)
{
    inputFiles = value;
}
QDateTime Info::getTimeOfConversion() const
{
    return timeOfConversion;
}

void Info::setTimeOfConversion(const QDateTime &value)
{
    timeOfConversion = value;
}
std::string Info::getTrackingInfoAlgorithm() const
{
    return trackingInfoAlgorithm;
}

void Info::setTrackingInfoAlgorithm(const std::string &value)
{
    trackingInfoAlgorithm = value;
}
std::string Info::getTrackingInfoILastikVersion() const
{
    return trackingInfoILastikVersion;
}

void Info::setTrackingInfoILastikVersion(const std::string &value)
{
    trackingInfoILastikVersion = value;
}
QDateTime Info::getTrackingInfoTimeOfTracking() const
{
    return trackingInfoTimeOfTracking;
}

void Info::setTrackingInfoTimeOfTracking(const QDateTime &value)
{
    trackingInfoTimeOfTracking = value;
}

}

std::ostream &operator<<(std::ostream &strm, const CellTracker::Info &i)
{
    strm << "Info:" << std::endl;
    strm << "  inputFiles:" << std::endl;
    for (std::string s: i.inputFiles)
        strm << "    " << s << std::endl;
    strm << "  timeOfConversion: " << i.timeOfConversion.toString().toStdString() << std::endl;
    strm << "  algorithm: " << i.trackingInfoAlgorithm << std::endl;
    strm << "  ilastik_version: " << i.trackingInfoILastikVersion << std::endl;
    strm << "  timeOfTracking: " << i.trackingInfoTimeOfTracking.toString().toStdString() << std::endl;
    return strm;
}

