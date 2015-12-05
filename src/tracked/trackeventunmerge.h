#ifndef TRACKEVENTUNMERGE_H
#define TRACKEVENTUNMERGE_H

#include "trackevent.h"

#include <memory>

#include <QList>

#include "tracklet.h"

namespace CellTracker {

/*!
 * \brief The TrackEventUnmerge class
 *
 * The TrackEventUnmerge class should be used, when two previously merged Tracklet%s
 * (see TrackEventMerge) become separate tracks again.
 *
 * An instance of this class holds a Tracklet prev which is the previous
 * Tracklet and a QList of Tracklet%s that are the following Tracklets.
 */
template <typename T>
class TrackEventUnmerge : public TrackEvent<T>
{
    friend std::ostream& ::operator<< <>(std::ostream&, CellTracker::TrackEvent<T>&);
public:
    TrackEventUnmerge();

    std::shared_ptr<T> getPrev() const;
    void setPrev(const std::shared_ptr<T> &value);

    std::shared_ptr<QList<std::shared_ptr<T>>> getNext() const;
    void setNext(const std::shared_ptr<QList<std::shared_ptr<T> > > &value);

private:
    std::shared_ptr<T> prev;                         /*!< The previously merged Track */
    std::shared_ptr<QList<std::shared_ptr<T>>> next; /*!< The unmerged Track%s */
};

}

#endif // TRACKEVENTUNMERGE_H
