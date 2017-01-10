#ifndef TRACKEVENTENDOFMOVIE_H
#define TRACKEVENTENDOFMOVIE_H

#include "trackevent.h"

#include <memory>

#include "tracklet.h"

namespace CellTracker {

/*!
 * \brief The TrackEventEndOfMovie class
 *
 * This class should be used, when a tracklet reaches the end of the movie, ie.
 * doesn't have to be tracked anymore.
 *
 * Consequently there is no member next, but only prev.
 */
template <typename T>
class TrackEventEndOfMovie : public TrackEvent<T>
{
    friend std::ostream& ::operator<< <>(std::ostream&, CellTracker::TrackEvent<T>&);
public:
    /*!
     * \brief constructs a new TrackEventEndOfMovie
     */
    TrackEventEndOfMovie() : TrackEvent<T>(TrackEvent<T>::EVENT_TYPE_ENDOFMOVIE) {}

    /*!
     * \brief returns the previous Tracklet
     * \return the previous Tracklet
     */
    std::weak_ptr<T> getPrev() const {
        return prev;
    }
    /*!
     * \brief sets the previous Tracklet
     * \param value the Tracklet to set as previous
     */
    void setPrev(const std::weak_ptr<T> &value) {
        prev = value;
    }

private:
    std::weak_ptr<T> prev; /*!< The previous Tracklet who reached the end of the movie */
};

}

#endif // TRACKEVENTENDOFMOVIE_H
