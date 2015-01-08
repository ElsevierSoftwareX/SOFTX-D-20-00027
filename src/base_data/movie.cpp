#include "movie.h"
#include <string>

Movie::Movie()
{
}

Movie::~Movie()
{
}

void Movie::addFrame(std::shared_ptr<Frame> f)
{
    return frames.append(f);
}

std::shared_ptr<Frame> Movie::getFrame(uint32_t id)
{
/*    if (id >=0 && id < frames.size()){ // in case we use int */
    if (static_cast<int>(id) < frames.size()){
        std::shared_ptr<Frame> f(frames.at(id));
        if(f->getID() == id)
            return f;
    }

    /* unordered list */
    for (std::shared_ptr<Frame> f: frames)
        if(f->getID() == id)
            return f;

    /* searched everything and found nothing */
    return nullptr;
}

std::ostream& operator<<(std::ostream &strm, const Movie &m)
{
    strm << "Movie:" << std::endl;
    strm << "  ilastikVersion: \"" << m.ilastikVersion << "\"" << std::endl;
    strm << "  inputFiles: \"" << m.inputFiles << "\"" << std::endl;
    strm << "  timeOfConversion: \"" << m.timeOfConversion << "\"" << std::endl;
    strm << "  timeOfTracking: \"" << m.timeOfTracking << "\"" << std::endl;
    strm << "  trackingAlgorithm: \"" << m.trackingAlgorithm << "\"" << std::endl;
    strm << "  frames:" << std::endl;
    for (std::shared_ptr<Frame> f : m.frames){
        strm << *f;
    }
    return strm;
}