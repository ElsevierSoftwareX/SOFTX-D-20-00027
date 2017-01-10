#ifndef TRACKEVENTDIVISION_H
#define TRACKEVENTDIVISION_H

#include "trackevent.h"

#include <memory>

#include <QList>

#include "tracklet.h"

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
    friend std::ostream& ::operator<< <>(std::ostream&, CellTracker::TrackEvent<T>&);
public:
    /*!
     * \brief constructs a new TrackEventDivision
     */
    TrackEventDivision() : TrackEvent<T>(TrackEvent<T>::EVENT_TYPE_DIVISION),
                           next(std::make_shared<QList<std::weak_ptr<T>>>()) {}

    /*!
     * \brief returns the previous (Auto)Tracklet
     * \return the previous (Auto)Tracklet
     */
    std::weak_ptr<T> getPrev() const {
        return prev;
    }
    /*!
     * \brief sets the previous (Auto)Tracklet
     * \param value the previous (Auto)Tracklet to set
     */
    void setPrev(const std::weak_ptr<T> &value) {
        prev = value;
    }

    /*!
     * \brief returns a QList of next (Auto)Tracklet%s
     * \return a Qlist of next (Auto)Tracklet%s
     */
    std::shared_ptr<QList<std::weak_ptr<T> > > getNext() const {
        return next;
    }
    /*!
     * \brief sets the QList of next (Auto)Tracklet%s
     * \param value the QList of next (Auto)Tracklet%s to set
     */
    void setNext(const std::shared_ptr<QList<std::weak_ptr<T>>> &value) {
        next = value;
    }

private:
    std::weak_ptr<T> prev;                         /*!< The previous Tracklet */
    std::shared_ptr<QList<std::weak_ptr<T>>> next; /*!< The list of next Tracklets */
};

}
#endif // TRACKEVENTDIVISION_H
