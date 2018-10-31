/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2016, 2015 Sebastian Wagner
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
#ifndef MOVIE_H
#define MOVIE_H

#include <cstdint>
#include <iostream>
#include <memory>

#include <QHash>

#include "frame.h"

namespace CellTracker { class Movie; }
std::ostream& operator<<(std::ostream&, const CellTracker::Movie&);

namespace CellTracker {

/*!
 * \brief The Movie class
 *
 * A Movie object consists of Frame%s which in turn represent certain points in
 * time.
 */
class Movie
{
public:
    Movie() = default;
    ~Movie() = default;

    std::shared_ptr<Frame> getFrame(uint32_t) const;
    QHash<uint32_t,std::shared_ptr<Frame>> getFrames();

    void addFrame(std::shared_ptr<Frame>);

    friend std::ostream& ::operator<< (std::ostream&, const CellTracker::Movie&);

private:
    QHash<uint32_t,std::shared_ptr<Frame>> frames; /*!< The Frames contained in this Movie */
};

}

#endif // MOVIE_H
