#ifndef FRAME_H
#define FRAME_H

#include <cstdint>
#include <QPoint>

#include "slice.h"

class Frame
{
public:
    Frame();

private:
    uint32_t id;
    QPoint slicesDim;
    QList<Slice> slices;
};

#endif // FRAME_H
