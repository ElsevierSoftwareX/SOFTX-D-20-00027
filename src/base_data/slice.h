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

private:
    QPoint slicePos;
    QList<Channel> channels;
};

#endif // SLICE_H
