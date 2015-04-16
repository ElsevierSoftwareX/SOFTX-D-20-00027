#ifndef TRACKEVENTMERGE_H
#define TRACKEVENTMERGE_H

#include "trackevent.h"

#include <memory>

#include <QList>

#include "tracklet.h"

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
template <typename T>
class TrackEventMerge : public TrackEvent<T>
{
    friend std::ostream& ::operator<< <>(std::ostream&, CellTracker::TrackEvent<T>&);
public:
    TrackEventMerge();

    std::shared_ptr<QList<std::shared_ptr<T> > > getPrev() const;
    void setPrev(const std::shared_ptr<QList<std::shared_ptr<T> > > &value);

    std::shared_ptr<T> getNext() const;
    void setNext(const std::shared_ptr<T> &value);

    void blaFunc();

private:
    std::shared_ptr<QList<std::shared_ptr<T>>> prev; /*!< The list of Tracklets that merge */
    std::shared_ptr<T> next;                         /*!< The combined Tracklet */
};

}

#endif // TRACKEVENTMERGE_H
