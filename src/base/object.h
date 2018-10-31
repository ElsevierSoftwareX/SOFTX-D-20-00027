/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2017, 2016, 2015 Sebastian Wagner
 *
 * Celltracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Celltracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Celltracker.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef OBJECT_H
#define OBJECT_H

#include "tracked/annotateable.h"

#include <cstdint>
#include <iostream>
#include <memory>

#include <QPoint>
#include <QPolygonF>
#include <QRect>

#include "channel.h"

namespace CellTracker { class Object; class Channel; }
std::ostream& operator<< (std::ostream&, CellTracker::Object&);

namespace CellTracker {

/*!
 * \brief The Object class
 *
 * An Object represents something that is automatically recognized in the Image
 * (i.e. Channel) that it is contained in.
 */
class Object : public Annotateable
{
public:
    Object() __attribute__((__deprecated__));
    Object(std::shared_ptr<Channel> channel_);
    Object(uint32_t id_, std::shared_ptr<Channel> channel_);
    ~Object() = default;

    uint32_t getId() const;
    uint32_t getTrackId() const;
    uint32_t getChannelId() const;
    uint32_t getSliceId() const;
    uint32_t getFrameId() const;
    uint32_t getAutoId() const;

    bool isInTracklet() const;
    bool isInAutoTracklet() const;

    std::shared_ptr<QRect> getBoundingBox() const;
    std::shared_ptr<QPolygonF> getOutline() const;
    std::shared_ptr<QPoint> getCentroid() const;

    void setId(uint32_t value);
    void setTrackId(const uint32_t &trackId);
    void setAutoId(const uint32_t &);

    void setCentroid(std::shared_ptr<QPoint>);
    void setBoundingBox(std::shared_ptr<QRect>);
    void setOutline(std::shared_ptr<QPolygonF>);

    friend std::ostream& ::operator<< (std::ostream&, Object&);

private:
    uint32_t id;                        /*!< The ID of this Object */
    std::weak_ptr<Channel> channel;
    uint32_t trackId;                   /*!< The ID of the Tracklet, that this Object is associated with (or UINT32_MAX) */
    uint32_t autoId;                    /*!< The ID of the AutoTracket, that this Object is associated with (or UINT32_MAX) */
    std::shared_ptr<QPoint> centroid;   /*!< The center of this Object */
    std::shared_ptr<QRect> boundingBox; /*!< The boundingBox of this Object */
    std::shared_ptr<QPolygonF> outline; /*!< The outline of this Object */

};

}

#endif // OBJECT_H
