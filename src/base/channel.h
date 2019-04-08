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
#ifndef CHANNEL_H
#define CHANNEL_H

#include "object.h"

#include <iostream>
#include <string>
#include <memory>

#include <QImage>
#include <QHash>

namespace TraCurate { class Channel; class Object; }
std::ostream &operator<<(std::ostream&, const TraCurate::Channel&);

namespace TraCurate {

/*!
 * \brief The Channel class
 *
 * A Channel represents the image. Associated with a certain Frame/Slice. There
 * may be more than one image. It holds the automatically recognized objects
 * that can be seen in the this image.
 *
 * \warning currently, images are requested by the ImageProvider using the
 * TraCurate::ImportHDF5::requestImage()-function and not stored in the Channel
 */
class Channel
{
public:
    Channel() __attribute__((deprecated));
    Channel(uint32_t chanId_, uint32_t sliceId_, uint32_t frameId_);
    ~Channel() = default;

    std::shared_ptr<QImage> getImage() const;

    void setImage(const std::shared_ptr<QImage> &);

    friend std::ostream& ::operator<<(std::ostream&, const Channel&);

    void addObject(const std::shared_ptr<Object> &);
    int removeObject(uint32_t);
    std::shared_ptr<Object> getObject(uint32_t) const;
    QHash<uint32_t,std::shared_ptr<Object>> getObjects();

    uint32_t getChanId() const;
    uint32_t getSliceId() const;
    uint32_t getFrameId() const;

private:
    uint32_t chanId;                                 /*!< the ID of this Channel */
    uint32_t sliceId;                                /*!< the ID of the Slice, that this Channel belongs to */
    uint32_t frameId;                                /*!< the ID of the Frame, that this Channel belongs to */
    std::shared_ptr<QImage> image;                   /*!< the QImage that is associated with this Channel. Currently unused,
                                                        as images are loaded ad-hoc by the ImageProvider */
    QHash<uint32_t,std::shared_ptr<Object>> objects; /*!< the Object%s that can be seen in this Channel */
};

}

#endif // CHANNEL_H
