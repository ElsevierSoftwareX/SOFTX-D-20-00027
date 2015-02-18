#ifndef TRACKEVENTUNMERGE_H
#define TRACKEVENTUNMERGE_H

#include "trackevent.h"

#include <memory>

#include <QList>

#include "tracklet.h"

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

    std::shared_ptr<Tracklet> getPrev() const;
    void setPrev(const std::shared_ptr<Tracklet> &value);

    QList<std::shared_ptr<Tracklet> > getNext() const;
    void setNext(const QList<std::shared_ptr<Tracklet> > &value);

private:
    std::shared_ptr<Tracklet> prev;        /*!< The previously merged Track */
    QList<std::shared_ptr<Tracklet>> next; /*!< The unmerged Track%s */
};

#endif // TRACKEVENTUNMERGE_H
