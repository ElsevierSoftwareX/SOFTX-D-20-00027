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
    ~Slice();
    void addChannel(Channel *);
    QPoint *getSlicePos();
    Channel *getChannel(QString);
    friend std::ostream &operator<<(std::ostream&, const Slice&);

private:
    QPoint slicePos;
    QList<Channel*> channels;
};

std::ostream &operator<<(std::ostream&, const Slice &);

#endif // SLICE_H
