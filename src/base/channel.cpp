/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2017, 2016, 2015 Sebastian Wagner
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
#include "channel.h"

namespace TraCurate {

/*!
 * \brief constructor for Channel::Channel
 */
Channel::Channel() :
    chanId(UINT32_MAX),
    sliceId(UINT32_MAX),
    frameId(UINT32_MAX) {}

/*!
 * \brief constructor for Channel::Channel
 * \param chanId_ the ChannelID of this Channel
 * \param sliceId_ the SliceID of this Channel
 * \param frameId_ the FrameID of this Channel
 */
Channel::Channel(uint32_t chanId_, uint32_t sliceId_, uint32_t frameId_) :
    chanId(chanId_),
    sliceId(sliceId_),
    frameId(frameId_) {}

/*!
 * \brief sets the image for this Channel
 * \warning currently, images are requested by the ImageProvider using the
 * TraCurate::ImportHDF5::requestImage()-function and not stored in the Channel
 * \param img the Image
 */
void Channel::setImage(const std::shared_ptr<QImage> &img)
{
    image = img;
}

/*!
 * \brief returns the channelID of this Channel
 * \return the channelID
 */
uint32_t Channel::getChanId() const
{
    return this->chanId;
}

/*!
 * \brief returns the image of this Channel
 * \warning currently, images are requested by the ImageProvider using the
 * TraCurate::ImportHDF5::requestImage()-function and not stored in the Channel
 * \return the image
 */
std::shared_ptr<QImage> Channel::getImage() const
{
   return this->image;
}

/*!
 * \brief adds an Object to this Channel
 * \param o the Object
 */
void Channel::addObject(const std::shared_ptr<Object> &o)
{
    objects.insert(o->getId(),o);
}

/*!
 * \brief removes an Object from this Channel
 * \param id the ID of the Object to remove
 * \return the number of removed objects (should either be 0 or 1)
 */
int Channel::removeObject(uint32_t id)
{
    return objects.remove(id);
}

/*!
 * \brief returns an Object from this Channel
 * \param id the ID of the Object to return
 * \return the Object
 */
std::shared_ptr<Object> Channel::getObject(uint32_t id) const
{
    return objects.value(id,nullptr);
}

/*!
 * \brief returns all Objects of this Channel in a QHash, using the ObjectID as a key to it
 * \return a QHash of all Objects
 */
QHash<uint32_t,std::shared_ptr<Object>> Channel::getObjects()
{
    return objects;
}

/*!
 * \brief returns the sliceID of this Channel
 * \return the sliceID
 */
uint32_t Channel::getSliceId() const
{
    return sliceId;
}

/*!
 * \brief returns the frameID of this Channel
 * \return the frameID
 */
uint32_t Channel::getFrameId() const
{
    return frameId;
}

}

std::ostream &operator<<(std::ostream &strm, const TraCurate::Channel &c)
{
    strm << "            Channel:" << std::endl;
    strm << "              id: " << std::to_string(c.chanId) << std::endl;
    strm << "              image: " << c.image << std::endl;
    return strm;
}
