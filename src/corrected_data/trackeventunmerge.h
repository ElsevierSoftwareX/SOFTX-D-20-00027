#ifndef TRACKEVENTUNMERGE_H
#define TRACKEVENTUNMERGE_H

#include <QList>

#include "tracklet.h"
#include "trackevent.h"

/*!
 * \brief The TrackEventUnmerge class
 *
 * The TrackEventUnmerge class should be used, when two previously merged tracks
 * (see TrackEventMerge) become separate tracks again.
 *
 * An instance of this class holds a Tracklet prev which is the previous
 * Tracklet and a QList of Tracklet%s that are the following Tracklets.
 */
class TrackEventUnmerge : public TrackEvent
{
public:
    TrackEventUnmerge();

private:
    Tracklet prev;        /*!< The previously merged Track */
    QList<Tracklet> next; /*!< The unmerged Track%s */
};

#endif // TRACKEVENTUNMERGE_H
