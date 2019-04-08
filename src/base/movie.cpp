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
#include "movie.h"

namespace TraCurate {

/*!
 * \brief adds a Frame to the Movie
 * \param f the Frame to add
 */
void Movie::addFrame(std::shared_ptr<Frame> f)
{
    frames.insert(f->getID(),f);
}

/*!
 * \brief returns a Frame in a Movie
 * \param id the ID of the Frame to return
 * \return the Frame
 */
std::shared_ptr<Frame> Movie::getFrame(uint32_t id) const
{
    return frames.value(id, nullptr);
}

/*!
 * \brief returns a QHash of all Frames in the Movie
 *
 * This functions returns a QHash of all Frames in the Movie, where the FrameID
 * of the Frames is used as a key to the QHash
 *
 * \return the QHash of all Frames in the Movie
 */
QHash<uint32_t,std::shared_ptr<Frame>> Movie::getFrames()
{
    return frames;
}

}

std::ostream& operator<<(std::ostream &strm, const TraCurate::Movie &m)
{
    strm << "Movie:" << std::endl;
    strm << "  frames:" << std::endl;
    for (std::shared_ptr<TraCurate::Frame> f : m.frames){
        strm << *f;
    }
    return strm;
}
