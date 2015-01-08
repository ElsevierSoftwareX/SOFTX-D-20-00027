#ifndef TRACKEVENTDIVISION_H
#define TRACKEVENTDIVISION_H

#include <QList>

#include "tracklet.h"
#include "trackevent.h"

/*!
 * \brief The TrackEventDivision class
 *
 * TrackEventDivision is used in situations, when an Object actually divides
 * itself (e.g. cell division).
 *
 * Due to that it holds the previous Tracklet and a list of new Tracklets.
 */
class TrackEventDivision : public TrackEvent
{
public:
    TrackEventDivision();

    std::shared_ptr<Tracklet> getPrev() const;
    void setPrev(const std::shared_ptr<Tracklet> &value);

    QList<std::shared_ptr<Tracklet> > getNext() const;
    void setNext(const QList<std::shared_ptr<Tracklet> > &value);

private:
    std::shared_ptr<Tracklet> prev;        /*!< The previous Tracklet */
    QList<std::shared_ptr<Tracklet>> next; /*!< The list of new Tracklets */
};

#endif // TRACKEVENTDIVISION_H