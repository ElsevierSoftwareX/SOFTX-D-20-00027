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

//    int getId() const;
//    uint32_t getFrameId() const;

    std::shared_ptr<QPoint> getSlicePos() const;
    std::shared_ptr<Channel> getChannel(uint32_t) const;
//    std::shared_ptr<Object> getObject(uint32_t) const __attribute__((__deprecated__));
//    QHash<uint32_t,std::shared_ptr<Object>> getObjects() __attribute__((__deprecated__));

    void addChannel(std::shared_ptr<Channel>);
    void setSlicePos(const std::shared_ptr<QPoint> &value);
    void setSlicePos(int, int);
//    void addObject(const std::shared_ptr<Object> &) __attribute__((__deprecated__));
//    int removeObject(uint32_t) __attribute__((__deprecated__));

    friend std::ostream& ::operator<<(std::ostream&, const Slice&);

    QHash<uint32_t, std::shared_ptr<Channel> > getChannels() const;

    uint32_t getSliceId() const;
    uint32_t getFrameId() const;

private:
    uint32_t sliceId;
    uint32_t frameId;
    std::shared_ptr<QPoint> slicePos;
    QHash<uint32_t,std::shared_ptr<Channel>> channels;
//    QHash<uint32_t,std::shared_ptr<Object>> objects __attribute__((__deprecated__));
};

}

#endif // SLICE_H
