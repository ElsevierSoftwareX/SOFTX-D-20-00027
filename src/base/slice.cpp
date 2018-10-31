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
#include "slice.h"

#include <QDebug>

namespace CellTracker {

/*!
 * \brief constructs a Slice
 *
 * \warning Do not use this.
 */
Slice::Slice() : sliceId(UINT32_MAX), frameId(UINT32_MAX), slicePos(std::make_shared<QPoint>(0,0)) { }

/*!
 * \brief constructs a Slice
 *
 * \param id_ the SliceID of the Slice
 * \param frameId_ the FrameID of the Slice
 */
Slice::Slice(uint32_t id_, uint32_t frameId_) : sliceId(id_), frameId(frameId_), slicePos(std::make_shared<QPoint>(0,0)) {}

/*!
 * \brief adds a Channel to a Slice
 * \param channel the Channel to add to this Slice
 */
void Slice::addChannel(std::shared_ptr<Channel> channel)
{
    channels.insert(channel->getChanId(),channel);
}

/*!
 * \brief returns a Channel in this Slice
 * \param id the ID of the Channel
 * \return the Channel
 */
std::shared_ptr<Channel> Slice::getChannel(uint32_t id) const
{
    return channels.value(id,nullptr);
}

/*!
 * \brief returns the Position of the Slice
 * \return the Position of this Slice
 * \warning Do not use this.
 */
std::shared_ptr<QPoint> Slice::getSlicePos() const
{
    return slicePos;
}

/*!
 * \brief sets the Position of the Slice
 * \param value the Position of this Slice
 * \warning Do not use this.
 */
void Slice::setSlicePos(const std::shared_ptr<QPoint> &value)
{
    slicePos = value;
}

/*!
 * \brief sets the Position of the Slice using x- and y-coordinates
 * \param x the x-component of the Position of this Slice
 * \param y the y-component of the Position of this Slice
 * \warning Do not use this.
 */
void Slice::setSlicePos(int x, int y)
{
    slicePos = std::make_shared<QPoint>(x,y);
}

/*!
 * \brief returns all Channels of a Slice
 *
 * Returns a QHash of the Channels of this Slice using the SliceID as key
 * to the QHash
 *
 * \return the QHash containing the Channels
 */
QHash<uint32_t, std::shared_ptr<Channel> > Slice::getChannels() const
{
    return channels;
}

/*!
 * \brief returns the SliceID of this Channel
 * \return the sliceID of this Channel
 */
uint32_t Slice::getSliceId() const
{
    return sliceId;
}

/*!
 * \brief returns the FrameID of this Channel
 * \return the frameID of this Channel
 */
uint32_t Slice::getFrameId() const
{
    return frameId;
}

}

std::ostream &operator<<(std::ostream &strm, const CellTracker::Slice &s)
{
    strm << "          Slice:" << std::endl;
    strm << "            id: " << s.sliceId << std::endl;
    strm << "            slicePos: " << s.slicePos->x() << "," << s.slicePos->y() << std::endl;
    strm << "            channels:" << std::endl;
    for (std::shared_ptr<CellTracker::Channel> c: s.channels){
        strm << *c;
    }
    strm << "            objects:" << std::endl ;
    return strm;
}
