#ifndef INFO_H
#define INFO_H

#include <iostream>
#include <string>

#include <QList>
#include <QDateTime>

namespace CellTracker { class Info; }
std::ostream& operator<<(std::ostream &, const CellTracker::Info &);

namespace CellTracker {

class Info
{
public:
    Info();

    QList<std::string> getInputFiles() const;
    void setInputFiles(const QList<std::string> &value);

    QDateTime getTimeOfConversion() const;
    void setTimeOfConversion(const QDateTime &value);

    std::string getTrackingInfoAlgorithm() const;
    void setTrackingInfoAlgorithm(const std::string &value);

    std::string getTrackingInfoILastikVersion() const;
    void setTrackingInfoILastikVersion(const std::string &value);

    QDateTime getTrackingInfoTimeOfTracking() const;
    void setTrackingInfoTimeOfTracking(const QDateTime &value);

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
