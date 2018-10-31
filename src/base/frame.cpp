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
#include "frame.h"

namespace CellTracker {

/*!
 * \brief constructor for Frame::Frame
 *
 * Initializes id with UINT32_MAX to indicate, that it hasn't been set
 */
Frame::Frame() :
    id(UINT32_MAX) {}

/*!
 * \brief constructor for Frame::Frame
 * \param id_ the ID of this frame (equals the point in time)
 */
Frame::Frame(uint32_t id_) :
    id(id_) {}

/*!
 * \brief adds a Slice to this Frame
 * \param slice the slice to add
 */
void Frame::addSlice(std::shared_ptr<Slice> slice)
{
    return slices.append(slice);
}

/*!
 * \brief sets the ID of this Frame
 * \param value this ID to set
 */
void Frame::setID(uint32_t value)
{
    this->id = value;
}

/*!
 * \brief returns the ID of this Frame
 * \return the ID of this Frame
 */
uint32_t Frame::getID() const
{
    return id;
}

/*!
 * \brief returns the Slice with ID id
 * \param value the ID of the Slice to return
 * \return the Slice
 */
std::shared_ptr<Slice> Frame::getSlice(int value) const
{
    for (std::shared_ptr<Slice> s: slices) {
        if (value >= 0 && s->getSliceId() == static_cast<uint32_t>(value))
            return s;
    }
    return nullptr;
}

/*!
 * \brief returns all Slices of this Frame
 * \return the Slices in a QList
 */
QList<std::shared_ptr<Slice>> Frame::getSlices()
{
    return slices;
}
}

std::ostream &operator<<(std::ostream &strm, const CellTracker::Frame &f)
{
    strm << "      Frame:" << std::endl;
    strm << "        id: " << f.id << std::endl;
    strm << "        slicesDim: " << f.slicesDim.x() << "," << f.slicesDim.y() << std::endl;
    strm << "        slices:" << std::endl;
    for (std::shared_ptr<CellTracker::Slice> s: f.slices){
        strm << *s;
    }
    return strm;
}
