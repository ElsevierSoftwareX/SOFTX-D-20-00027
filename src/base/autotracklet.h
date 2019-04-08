/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2016, 2015 Sebastian Wagner
 *
 * TraCurate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TraCurate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with TraCurate.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef AUTOTRACKLET_H
#define AUTOTRACKLET_H

#include <iostream>
#include <memory>

#include <QMap>
#include <QPair>

#include "base/frame.h"
#include "base/object.h"
#include "tracked/trackevent.h"

namespace TraCurate { class AutoTracklet; }
std::ostream& operator<< (std::ostream&, TraCurate::AutoTracklet&);
std::ostream& operator<< (std::ostream&, QPair<std::shared_ptr<TraCurate::Frame>, std::shared_ptr<TraCurate::Object>>&);

namespace TraCurate {

/*!
 * \brief The AutoTracklet class
 *
 * Collects a list of Frame%ID/Object%-Pairs in an AutoTracklet.
 * These are the automatically generated tracks.
 */
class AutoTracklet
{
public:
    AutoTracklet() __attribute__((deprecated));
    AutoTracklet(int id);
    ~AutoTracklet();
    void addComponent(std::shared_ptr<Frame>, std::shared_ptr<Object>);
    void addComponent(QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>>);
    void removeComponent(int);
    void removeComponent(std::shared_ptr<Frame>);
    void setID(int) __attribute__((deprecated));
    int getID();

    int getStart();
    int getEnd();
    uint32_t getLength();

    /*! \todo nicer way to iterate over those */
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
