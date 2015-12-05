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
 * Collects a list of Frame%/Object%-Pairs in an AutoTracklet.
 * These are the automatically generated tracks.
 */
class AutoTracklet
{
public:
    AutoTracklet();
    void addComponent(std::shared_ptr<Frame>,std::shared_ptr<Object>);
    void addComponent(QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>);
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
    int trackID;
    QMap<int,std::shared_ptr<Object>> components; //
    std::shared_ptr<TrackEvent<AutoTracklet>> next;
    std::shared_ptr<TrackEvent<AutoTracklet>> prev;
};

}

#endif // AUTOTRACKLET_H
