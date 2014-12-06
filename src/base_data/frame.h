#ifndef FRAME_H
#define FRAME_H

#include <cstdint>
#include <QPoint>

#include "slice.h"

/*!
 * \brief The Frame class
 *
 * A Frame object represents a certain point in time. It (indirectly) holds all
 * the image data associated with that point in time. A Frame can be split up
 * into Slice%s, but it may also only hold one Slice.
 */
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
