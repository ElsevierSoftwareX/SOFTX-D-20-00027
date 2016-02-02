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
    TrackEventEndOfMovie();

    std::shared_ptr<T> getPrev() const;
    void setPrev(const std::shared_ptr<T> &value);

private:
    std::shared_ptr<T> prev; /*!< The previous Tracklet who reached the end of the movie */
};

}

#endif // TRACKEVENTENDOFMOVIE_H
