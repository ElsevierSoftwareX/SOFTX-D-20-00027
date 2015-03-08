#ifndef TRACKLETSPLIT_H
#define TRACKLETSPLIT_H

#include "tracklet.h"

#include "auto_tracklets/autotracklet.h"
#include "base_data/frame.h"

namespace CellTracker { class Tracklet; }
std::ostream& operator<< (std::ostream&, CellTracker::Tracklet&);

namespace CellTracker {

/*!
 * \brief The TrackletSplit class
 *
 * TrackletSplit describes the situation, when an AutoTracklet that was
 * automatically recognized is in fact one or more Tracklets.
 */
class TrackletSplit : public Tracklet
{
    friend std::ostream& ::operator<< (std::ostream&, Tracklet&);
public:
    TrackletSplit();

private:
    std::shared_ptr<AutoTracklet> baseTracklet;
    std::shared_ptr<Frame> from;
    std::shared_ptr<Frame> to;
};

}

#endif // TRACKLETSPLIT_H
