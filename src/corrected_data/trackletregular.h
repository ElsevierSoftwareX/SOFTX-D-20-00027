#ifndef TRACKLETREGULAR_H
#define TRACKLETREGULAR_H

#include "tracklet.h"

#include "auto_tracklets/autotracklet.h"

namespace CellTracker { class Tracklet; }
std::ostream& operator<< (std::ostream&, CellTracker::Tracklet&);

namespace CellTracker {

/*!
 * \brief The TrackletRegular class
 *
 * TrackletRegular is used in situations, when the automatically generated
 * Tracklet fits and nothing else has to be done.
 *
 * \deprecated Will be removed, tracklet-types are obsolete now
 */
class TrackletRegular : public Tracklet
{
    friend std::ostream& ::operator<< (std::ostream&, Tracklet&);
public:
    TrackletRegular();
    TrackletRegular(std::shared_ptr<AutoTracklet>);

private:
    std::shared_ptr<AutoTracklet> baseTracklet;
} __attribute_deprecated__;

}

#endif // TRACKLETREGULAR_H
