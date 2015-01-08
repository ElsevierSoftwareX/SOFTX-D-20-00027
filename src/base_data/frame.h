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
    Frame(uint32_t);
    ~Frame();
    void addSlice(std::shared_ptr<Slice>);
    void setID(uint32_t);
    uint32_t getID();
    std::shared_ptr<Slice> getSlice(int, int);
    friend std::ostream& operator<<(std::ostream&, const Frame&);

private:
    uint32_t id;
    QPoint slicesDim;
    QList<std::shared_ptr<Slice>> slices;
};

std::ostream& operator<<(std::ostream&, const Frame&);

#endif // FRAME_H