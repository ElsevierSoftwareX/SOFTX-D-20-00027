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

std::shared_ptr<Channel> Slice::getChannel(QString name)
{
    for (std::shared_ptr<Channel> c: channels)
        if(c->getChanName() == name)
            return c;
    return nullptr;
}
std::shared_ptr<QPoint> Slice::getSlicePos() const
{
    return slicePos;
}

void Slice::setSlicePos(const std::shared_ptr<QPoint> &value)
{
    slicePos = value;
}

void Slice::setSlicePos(int x, int y)
{
    slicePos = std::shared_ptr<QPoint>(new QPoint(x,y));
}


std::ostream &operator<<(std::ostream &strm, const Slice &s)
{
    strm << "          Slice:" << std::endl;
    strm << "            slicePos: " << s.slicePos->x() << "," << s.slicePos->y() << std::endl;
    strm << "            channels:" << std::endl;
    for (std::shared_ptr<Channel> c: s.channels){
        strm << *c;
    }
   return strm;
}