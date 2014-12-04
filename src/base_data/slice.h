#ifndef SLICE_H
#define SLICE_H

#include <QPoint>
#include <QList>

#include "channel.h"

class Slice
{
public:
    Slice();

private:
    QPoint slicePos;
    QList<Channel> channels;
};

#endif // SLICE_H
