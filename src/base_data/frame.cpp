#include "frame.h"

Frame::Frame()
{
}

Frame::~Frame()
{
    for (Slice *s:slices)
        delete s;
}

void Frame::addSlice(Slice *slice)
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

Slice *Frame::getSlice(int x, int y)
{
    /* TODO: Inefficient */
    for(Slice *s: slices){
        if (s->getSlicePos()->x() == x &&
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
    for (Slice *s: f.slices){
        strm << *s;
    }
    return strm;
}
