#include "movie.h"

namespace CellTracker {

Movie::Movie() {}

Movie::~Movie() {}

void Movie::addFrame(std::shared_ptr<Frame> f)
{
    frames.insert(f->getID(),f);
}

std::shared_ptr<Frame> Movie::getFrame(uint32_t id) const
{
    return frames.value(id, nullptr);
}

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
