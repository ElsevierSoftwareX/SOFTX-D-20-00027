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
#ifndef FRAME_H
#define FRAME_H

#include <cstdint>
#include <iostream>
#include <memory>

#include <QList>
#include <QPoint>

#include "slice.h"

namespace CellTracker { class Frame; }
std::ostream& operator<<(std::ostream&, const CellTracker::Frame&);

namespace CellTracker {

/*!
 * \brief The Frame class
 *
 * A Frame object represents a certain point in time. A Frame can be split up
 * into Slice%s, but it may also only hold one Slice.
 *
 * \warning Slices are currently quite unsupported, but if you want to add
 * support, this class and CellTracker::Slice is the place to work on.
 */
class Frame
{
public:
    Frame();
    Frame(uint32_t id_);
    ~Frame() = default;

    uint32_t getID() const;
    std::shared_ptr<Slice> getSlice(int) const;
    QList<std::shared_ptr<Slice>> getSlices();

    void setID(uint32_t);
    void addSlice(std::shared_ptr<Slice>);

    friend std::ostream& ::operator<<(std::ostream&, const Frame&);

private:
    uint32_t id;                          /*!< the ID of this Frame */
    QPoint slicesDim;                     /*!< How the slices of this Frame are arranged */
    QList<std::shared_ptr<Slice>> slices; /*!< the Slices that belong to this Frame */
};

}

#endif // FRAME_H
