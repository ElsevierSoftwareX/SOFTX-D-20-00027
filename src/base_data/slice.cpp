#include "slice.h"

Slice::Slice()
{
    this->slicePos = std::shared_ptr<QPoint>(new QPoint());
}

Slice::Slice(int x,int y)
{
    this->slicePos = std::shared_ptr<QPoint>(new QPoint(x,y));
}

Slice::~Slice()
{
}

void Slice::addChannel(std::shared_ptr<Channel> channel)
{
    return channels.append(channel);
}

std::shared_ptr<QPoint> Slice::getSlicePos()
{
    return slicePos;
}

std::shared_ptr<Channel> Slice::getChannel(QString name)
{
    for (std::shared_ptr<Channel> c: channels)
        if(c->getChanName() == name)
            return c;
    return nullptr;
}

std::ostream &operator<<(std::ostream &strm, const Slice &s)
{
    strm << "          Slice:\n";
    strm << "            slicePos: " << s.slicePos->x() << "," << s.slicePos->y() << "\n";
    strm << "            channels:\n";
    for (std::shared_ptr<Channel> c: s.channels){
        strm << *c;
    }
   return strm;
}
