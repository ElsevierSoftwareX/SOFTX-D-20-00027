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
    std::shared_ptr<Frame> f(frames.at(id));
    if(f->getID() != id)
        return nullptr;
    return f;
}

std::ostream& operator<<(std::ostream &strm, const Movie &m)
{
    strm << "Movie:\n";
    strm << "  ilastikVersion: \"" << m.ilastikVersion << "\"\n";
    strm << "  inputFiles: \"" << m.inputFiles << "\"\n";
    strm << "  timeOfConversion: \"" << m.timeOfConversion << "\"\n";
    strm << "  timeOfTracking: \"" << m.timeOfTracking << "\"\n";
    strm << "  trackingAlgorithm: \"" << m.trackingAlgorithm << "\"\n";
    strm << "  frames:\n";
    for (std::shared_ptr<Frame> f : m.frames){
        strm << *f;
    }
    return strm;
}
