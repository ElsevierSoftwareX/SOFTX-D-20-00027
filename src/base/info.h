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
    Info();

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
