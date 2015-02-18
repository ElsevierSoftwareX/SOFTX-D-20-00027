#ifndef TRACKLETMERGED_H
#define TRACKLETMERGED_H

#include <QList>

#include "tracklet.h"
#include "auto_tracklets/autotracklet.h"

/*!
 * \brief The TrackletMerged class
 *
 * TrackletMerged is used, when a Tracklet consists of two or more underlying
 * AutoTracklet%s.
 */
class TrackletMerged : public Tracklet
{
public:
    TrackletMerged();

private:
    QList<AutoTracklet> containedTracklets;
};

#endif // TRACKLETMERGED_H
