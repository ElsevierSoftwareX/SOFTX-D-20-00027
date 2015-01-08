#include "frame.h"

Frame::Frame()
{
}

Frame::Frame(uint32_t id)
{
    this->id = id;
}

Frame::~Frame()
{
}

void Frame::addSlice(std::shared_ptr<Slice> slice)
{
    return slices.append(slice);
}

void Frame::setID(uint32_t id)
{
    this->id = id;
}

uint32_t Frame::getID()
{
    return id;
}
#include <iostream>
std::shared_ptr<Slice> Frame::getSlice(int x, int y)
{
    for(std::shared_ptr<Slice> s: slices){
        if (s->getSlicePos() != nullptr &&
            s->getSlicePos()->x() == x &&
            s->getSlicePos()->y() == y)
            return s;
    }
    return nullptr;
}


std::ostream &operator<<(std::ostream &strm, const Frame &f)
{
    strm << "      Frame:" << std::endl;
    strm << "        id: " << f.id << std::endl;
    strm << "        slicesDim: " << f.slicesDim.x() << "," << f.slicesDim.y() << std::endl;
    strm << "        slices:" << std::endl;
    for (std::shared_ptr<Slice> s: f.slices){
        strm << *s;
    }
    return strm;
}
