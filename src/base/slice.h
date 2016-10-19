#ifndef SLICE_H
#define SLICE_H

#include <cstdint>
#include <iostream>

#include <QPoint>
#include <QHash>

#include "channel.h"
#include "object.h"

namespace CellTracker { class Slice; }
std::ostream &operator<<(std::ostream&, const CellTracker::Slice &);

namespace CellTracker {

/*!
 * \brief The Slice class
 *
 * A Slice is a part of an Image. For each Slice there can be one or more
 * Channel%s.
 *
 * \warning Currently more than one Slice is unsupported. Also anything having
 * to do with Slice Positions is most likely broken.
 */
class Slice
{
public:
    Slice() __attribute__((deprecated));
    Slice(uint32_t sliceId, uint32_t frameId);
    ~Slice() = default;

    std::shared_ptr<QPoint> getSlicePos() const __attribute__((deprecated));
    std::shared_ptr<Channel> getChannel(uint32_t) const;

    void addChannel(std::shared_ptr<Channel>);
    void setSlicePos(const std::shared_ptr<QPoint> &value) __attribute__((deprecated));
    void setSlicePos(int, int) __attribute__((deprecated));

    friend std::ostream& ::operator<<(std::ostream&, const Slice&);

    QHash<uint32_t, std::shared_ptr<Channel> > getChannels() const;

    uint32_t getSliceId() const;
    uint32_t getFrameId() const;

private:
    uint32_t sliceId;                                  /*!< the ID of this Slice */
    uint32_t frameId;                                  /*!< the ID of this Frame */
    std::shared_ptr<QPoint> slicePos;                  /*!< the Position of this Slice in the Frame */
    QHash<uint32_t,std::shared_ptr<Channel>> channels; /*!< the Channel%s contained in this Slice */
};

}

#endif // SLICE_H
