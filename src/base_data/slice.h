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
    Slice(int,int);
    ~Slice();
    void addChannel(std::shared_ptr<Channel>);
    std::shared_ptr<Channel> getChannel(QString);
    friend std::ostream &operator<<(std::ostream&, const Slice&);

    std::shared_ptr<QPoint> getSlicePos() const;
    void setSlicePos(const std::shared_ptr<QPoint> &value);
    void setSlicePos(int, int);

private:
    std::shared_ptr<QPoint> slicePos;
    QList<std::shared_ptr<Channel>> channels;
};

std::ostream &operator<<(std::ostream&, const Slice &);

#endif // SLICE_H
