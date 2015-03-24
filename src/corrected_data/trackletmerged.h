#ifndef TRACKLETMERGED_H
#define TRACKLETMERGED_H

#include "tracklet.h"

#include <QList>

#include "auto_tracklets/autotracklet.h"

namespace CellTracker { class Tracklet; }
std::ostream& operator<< (std::ostream&, CellTracker::Tracklet&);

namespace CellTracker {

/*!
 * \brief The TrackletMerged class
 *
 * TrackletMerged is used, when a Tracklet consists of two or more underlying
 * AutoTracklet%s.
 *
 * \deprecated Will be removed, tracklet-subclasses are obsolete now
 */
class TrackletMerged : public Tracklet
{
    friend std::ostream& ::operator<< (std::ostream&, Tracklet&);
public:
    TrackletMerged();

private:
    QList<AutoTracklet> containedTracklets;
}; //__attribute_deprecated__;

}

#endif // TRACKLETMERGED_H
