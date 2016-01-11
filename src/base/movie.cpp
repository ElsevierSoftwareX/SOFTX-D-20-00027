#include "movie.h"

namespace CellTracker {

/*!
 * \brief constructs a Movie
 */
Movie::Movie() {}

/*!
 * \brief desctructs a Movie
 */
Movie::~Movie() {}

/*!
 * \brief adds a Frame to the Movie
 * \param f the Frame to add
 */
void Movie::addFrame(std::shared_ptr<Frame> f)
{
    frames.insert(f->getID(),f);
}

/*!
 * \brief returns a Frame in a Movie
 * \param id the ID of the Frame to return
 * \return the Frame
 */
std::shared_ptr<Frame> Movie::getFrame(uint32_t id) const
{
    return frames.value(id, nullptr);
}

/*!
 * \brief returns a QHash of all Frames in the Movie
 *
 * This functions returns a QHash of all Frames in the Movie, where the FrameID
 * of the Frames is used as a key to the QHash
 *
 * \return the QHash of all Frames in the Movie
 */
QHash<uint32_t,std::shared_ptr<Frame>> Movie::getFrames()
{
    return frames;
}

}

std::ostream& operator<<(std::ostream &strm, const CellTracker::Movie &m)
{
    strm << "Movie:" << std::endl;
    strm << "  frames:" << std::endl;
    for (std::shared_ptr<CellTracker::Frame> f : m.frames){
        strm << *f;
    }
    return strm;
}
