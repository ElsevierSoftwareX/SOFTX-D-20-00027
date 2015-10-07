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
 */
class Slice
{
public:
    Slice();
    Slice(uint32_t sliceId, uint32_t frameId);
    ~Slice();

    std::shared_ptr<QPoint> getSlicePos() const;
    std::shared_ptr<Channel> getChannel(uint32_t) const;

    void addChannel(std::shared_ptr<Channel>);
    void setSlicePos(const std::shared_ptr<QPoint> &value);
    void setSlicePos(int, int);

    friend std::ostream& ::operator<<(std::ostream&, const Slice&);

    QHash<uint32_t, std::shared_ptr<Channel> > getChannels() const;

    uint32_t getSliceId() const;
    uint32_t getFrameId() const;

private:
    uint32_t sliceId;
    uint32_t frameId;
    std::shared_ptr<QPoint> slicePos;
    QHash<uint32_t,std::shared_ptr<Channel>> channels;
};

}

#endif // SLICE_H