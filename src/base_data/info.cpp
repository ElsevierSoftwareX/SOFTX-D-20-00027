#include "info.h"

#include <iostream>

Info::Info()
{
}
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

std::ostream &operator<<(std::ostream &strm, const Info &i)
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



