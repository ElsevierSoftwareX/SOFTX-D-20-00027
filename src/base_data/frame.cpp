#include "frame.h"

Frame::Frame()
{
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
    /*! \todo Inefficient */
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
    strm << "      Frame:\n";
    strm << "        id: " << f.id << "\n";
    strm << "        slicesDim: " << f.slicesDim.x() << "," << f.slicesDim.y() << "\n";
    strm << "        slices:\n";
    for (std::shared_ptr<Slice> s: f.slices){
        strm << *s;
    }
    return strm;
}
