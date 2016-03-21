#ifndef AUTOTRACKLET_H
#define AUTOTRACKLET_H

#include <iostream>
#include <memory>

#include <QMap>

#include "base/frame.h"
#include "base/object.h"
#include "tracked/trackevent.h"

namespace CellTracker { class AutoTracklet; }
std::ostream& operator<< (std::ostream&, CellTracker::AutoTracklet&);
std::ostream& operator<< (std::ostream&, QPair<std::shared_ptr<CellTracker::Frame>, std::shared_ptr<CellTracker::Object>>&);

namespace CellTracker {

/*!
 * \brief The AutoTracklet class
 *
 * Collects a list of Frame%ID/Object%-Pairs in an AutoTracklet.
 * These are the automatically generated tracks.
 */
class AutoTracklet
{
public:
    AutoTracklet();
    void addComponent(std::shared_ptr<Frame>,std::shared_ptr<Object>);
    void addComponent(QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>);
    void removeComponent(int);
    void removeComponent(std::shared_ptr<Frame>);
    void setID(int);
    int getID();

    int getStart();
    int getEnd();
    uint32_t getLength();

    QMap<int, std::shared_ptr<Object>> getComponents() const;
    friend std::ostream& ::operator<< (std::ostream&, AutoTracklet&);

    std::shared_ptr<TrackEvent<AutoTracklet> > getNext() const;
    void setNext(const std::shared_ptr<TrackEvent<AutoTracklet> > &value);

    std::shared_ptr<TrackEvent<AutoTracklet> > getPrev() const;
    void setPrev(const std::shared_ptr<TrackEvent<AutoTracklet> > &value);

private:
    int trackID;                                    /*!< the ID of this AutoTracklet */
    QMap<int,std::shared_ptr<Object>> components;   /*!< the components (i.e. pairs of frameId + Object%s) contained in this Tracklet */
    std::shared_ptr<TrackEvent<AutoTracklet>> next; /*!< the TrackEvent, that follows this AutoTracklet */
    std::shared_ptr<TrackEvent<AutoTracklet>> prev; /*!< the TrackEvent, that preceeds this AutoTracklet */
};

}

#endif // AUTOTRACKLET_H
