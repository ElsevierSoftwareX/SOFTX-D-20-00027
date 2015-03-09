#ifndef TRACKEVENTMERGE_H
#define TRACKEVENTMERGE_H

#include "trackevent.h"

#include <memory>

#include <QList>

#include "tracklet.h"

namespace CellTracker { class TrackEvent; }
std::ostream& operator<< (std::ostream&, CellTracker::TrackEvent&);

namespace CellTracker {

/*!
 * \brief The TrackEventMerge class
 *
 * The TrackEventMerge class is used for cases in which two separate objects are
 * recognized as a single one. When they split TrackEventUnmerge is supposed to
 * be used.
 *
 * Instances of this class hold a QList of Tracklets that merge together and a
 * Tracklet that represents the merged Objects.
 */
class TrackEventMerge : public TrackEvent
{
    friend std::ostream& ::operator<< (std::ostream&, TrackEvent&);
public:
    TrackEventMerge();

    QList<std::shared_ptr<Tracklet> > getPrev() const;
    void setPrev(const QList<std::shared_ptr<Tracklet> > &value);

    std::shared_ptr<Tracklet> getNext() const;
    void setNext(const std::shared_ptr<Tracklet> &value);

private:
    QList<std::shared_ptr<Tracklet>> prev; /*!< The list of Tracklets that merge */
    std::shared_ptr<Tracklet> next;        /*!< The combined Tracklet */
};

}

#endif // TRACKEVENTMERGE_H
