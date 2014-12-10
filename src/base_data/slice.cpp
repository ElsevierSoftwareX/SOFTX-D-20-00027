#include "slice.h"

Slice::Slice()
{
}

Slice::~Slice()
{
    for (Channel *c: channels)
        delete c;
}

void Slice::addChannel(Channel *channel)
{
    return channels.append(channel);
}

QPoint *Slice::getSlicePos()
{
    return &slicePos;
}

Channel *Slice::getChannel(QString name)
{
    for (Channel *c: channels)
        if(c->getChanName() == name)
            return c;
    return nullptr;
}

std::ostream &operator<<(std::ostream &strm, const Slice &s)
{
    strm << "          Slice:\n";
    strm << "            slicePos: " << s.slicePos.x() << "," << s.slicePos.y() << "\n";
    strm << "            channels:\n";
    for (Channel *c: s.channels){
        strm << *c;
    }
   return strm;
}
