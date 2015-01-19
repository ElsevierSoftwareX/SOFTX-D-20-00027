#ifndef SLICE_H
#define SLICE_H

#include <QPoint>
#include <QList>

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

    void addChannel(std::shared_ptr<Channel>);
    void setSlicePos(const std::shared_ptr<QPoint> &value);
    void setSlicePos(int, int);

    friend std::ostream &operator<<(std::ostream&, const Slice&);

private:
    int id;
    std::shared_ptr<QPoint> slicePos;
    QList<std::shared_ptr<Channel>> channels;
};

std::ostream &operator<<(std::ostream&, const Slice &);

#endif // SLICE_H
