#ifndef SLICE_H
#define SLICE_H

#include <QPoint>
#include <QHash>

#include "channel.h"

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
    Slice(int);
    ~Slice();

    int getId() const;
    std::shared_ptr<QPoint> getSlicePos() const;
    std::shared_ptr<Channel> getChannel(int) const;
    std::shared_ptr<Object> getObject(uint32_t) const;

    void addChannel(std::shared_ptr<Channel>);
    void setSlicePos(const std::shared_ptr<QPoint> &value);
    void setSlicePos(int, int);
    void addObject(const std::shared_ptr<Object>);

    friend std::ostream &operator<<(std::ostream&, const Slice&);

private:
    int id;
    std::shared_ptr<QPoint> slicePos;
    QHash<int,std::shared_ptr<Channel>> channels;
    QHash<uint32_t,std::shared_ptr<Object>> objects;
};

std::ostream &operator<<(std::ostream&, const Slice &);

#endif // SLICE_H
