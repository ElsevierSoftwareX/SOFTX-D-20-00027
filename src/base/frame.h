#ifndef FRAME_H
#define FRAME_H

#include <cstdint>
#include <iostream>
#include <memory>

#include <QList>
#include <QPoint>

#include "slice.h"

namespace CellTracker { class Frame; }
std::ostream& operator<<(std::ostream&, const CellTracker::Frame&);

namespace CellTracker {

/*!
 * \brief The Frame class
 *
 * A Frame object represents a certain point in time. A Frame can be split up
 * into Slice%s, but it may also only hold one Slice.
 */
class Frame
{
public:
    Frame();
    Frame(uint32_t id);
    ~Frame();

    uint32_t getID() const;
    std::shared_ptr<Slice> getSlice(int) const;
    std::shared_ptr<Slice> getSlice(int, int) const;
    QList<std::shared_ptr<Slice>> getSlices();

    void setID(uint32_t);
    void addSlice(std::shared_ptr<Slice>);

    friend std::ostream& ::operator<<(std::ostream&, const Frame&);

private:
    uint32_t id; /*!< the ID of this Frame */
    QPoint slicesDim; /*!< How the slices of this Frame are arranged */
    QList<std::shared_ptr<Slice>> slices; /*!< the Slices that belong to this Frame */
};

}

#endif // FRAME_H
