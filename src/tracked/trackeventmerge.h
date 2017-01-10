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
    /*!
     * \brief constructs a TrackEventMerge
     */
    TrackEventMerge() : TrackEvent<T>(TrackEvent<T>::EVENT_TYPE_MERGE),
                        prev(std::make_shared<QList<std::weak_ptr<T>>>()) {}


    /*!
     * \brief returns a QList of previous Tracklets
     * \return a QList of previous Tracklets
     */
    std::shared_ptr<QList<std::weak_ptr<T> > > getPrev() const {
        return prev;
    }
    /*!
     * \brief sets the QList of previous Tracklets
     * \param value the QList of previous Tracklets to set
     */
    void setPrev(const std::shared_ptr<QList<std::weak_ptr<T> > > &value) {
        prev = value;
    }

    /*!
     * \brief returns the next Tracklet
     * return the next Tracklet
     */
    std::weak_ptr<T> getNext() const {
        return next;
    }
    /*!
     * \brief sets the next Tracklet
     * \param value the next Tracklet to set
     */
    void setNext(const std::weak_ptr<T> &value) {
        next = value;
    }

private:
    std::shared_ptr<QList<std::weak_ptr<T>>> prev; /*!< The list of Tracklets that merge */
    std::weak_ptr<T> next;                         /*!< The combined Tracklet */
};

}

#endif // TRACKEVENTMERGE_H
