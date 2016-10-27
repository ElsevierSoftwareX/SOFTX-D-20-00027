#ifndef CHANNEL_H
#define CHANNEL_H

#include "object.h"

#include <iostream>
#include <string>
#include <memory>

#include <QImage>
#include <QHash>

namespace CellTracker { class Channel; class Object; }
std::ostream &operator<<(std::ostream&, const CellTracker::Channel&);

namespace CellTracker {

/*!
 * \brief The Channel class
 *
 * A Channel represents the image. Associated with a certain Frame/Slice. There
 * may be more than one image. It holds the automatically recognized objects
 * that can be seen in the this image.
 *
 * \warning currently, images are requested by the ImageProvider using the
 * CellTracker::ImportHDF5::requestImage()-function and not stored in the Channel
 */
class Channel
{
public:
    Channel() __attribute__((deprecated));
    Channel(uint32_t chanId, uint32_t sliceId, uint32_t frameId);
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
