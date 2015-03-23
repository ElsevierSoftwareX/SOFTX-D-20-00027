#ifndef TRACKEVENTDIVISION_H
#define TRACKEVENTDIVISION_H

#include "trackevent.h"

#include <memory>

#include <QList>

#include "tracklet.h"

namespace CellTracker { template <typename T> class TrackEvent; }
template <typename T> std::ostream& operator<< (std::ostream&, CellTracker::TrackEvent<T>&);

namespace CellTracker {

/*!
 * \brief The TrackEventDivision class
 *
 * TrackEventDivision is used in situations, when an Object actually divides
 * itself (e.g. cell division).
 *
 * Due to that it holds the previous Tracklet and a list of new Tracklets.
 */
template <typename T>
class TrackEventDivision : public TrackEvent<T>
{
    friend std::ostream& ::operator<< (std::ostream&, TrackEvent<T>&);
public:
    TrackEventDivision();

    std::shared_ptr<T> getPrev() const;
    void setPrev(const std::shared_ptr<T> &value);

    std::shared_ptr<QList<std::shared_ptr<T> > > getNext() const;
    void setNext(const std::shared_ptr<QList<std::shared_ptr<T>>> &value);

private:
    std::shared_ptr<T> prev;                         /*!< The previous Tracklet */
    std::shared_ptr<QList<std::shared_ptr<T>>> next; /*!< The list of new Tracklets */
};

}
#endif // TRACKEVENTDIVISION_H
