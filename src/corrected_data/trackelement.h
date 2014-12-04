#ifndef TRACKELEMENT_H
#define TRACKELEMENT_H

class TrackElement
{
    enum ELEMENT_TYPE {
        ELEMENT_TRACKLET,
        ELEMENT_EVENT
    };

public:
    TrackElement();

private:
    ELEMENT_TYPE elemType;
};

#endif // TRACKELEMENT_H
