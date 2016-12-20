#include "frame.h"

namespace CellTracker {

/*!
 * \brief constructor for Frame::Frame
 *
 * Initializes id with UINT32_MAX to indicate, that it hasn't been set
 */
Frame::Frame() :
    id(UINT32_MAX) {}

/*!
 * \brief constructor for Frame::Frame
 * \param id the ID of this frame (equals the point in time)
 */
Frame::Frame(uint32_t id) :
    id(id) {}

/*!
 * \brief adds a Slice to this Frame
 * \param slice the slice to add
 */
void Frame::addSlice(std::shared_ptr<Slice> slice)
{
    return slices.append(slice);
}

/*!
 * \brief sets the ID of this Frame
 * \param id this ID to set
 */
void Frame::setID(uint32_t id)
{
    this->id = id;
}

/*!
 * \brief returns the ID of this Frame
 * \return the ID of this Frame
 */
uint32_t Frame::getID() const
{
    return id;
}

/*!
 * \brief returns the Slice with ID id
 * \param id the ID of the Slice to return
 * \return the Slice
 */
std::shared_ptr<Slice> Frame::getSlice(int id) const
{
    for (std::shared_ptr<Slice> s: slices) {
        if (id >= 0 && s->getSliceId() == static_cast<uint32_t>(id))
            return s;
    }
    return nullptr;
}

/*!
 * \brief returns all Slices of this Frame
 * \return the Slices in a QList
 */
QList<std::shared_ptr<Slice>> Frame::getSlices()
{
    return slices;
}
}

std::ostream &operator<<(std::ostream &strm, const CellTracker::Frame &f)
{
    strm << "      Frame:" << std::endl;
    strm << "        id: " << f.id << std::endl;
    strm << "        slicesDim: " << f.slicesDim.x() << "," << f.slicesDim.y() << std::endl;
    strm << "        slices:" << std::endl;
    for (std::shared_ptr<CellTracker::Slice> s: f.slices){
        strm << *s;
    }
    return strm;
}
