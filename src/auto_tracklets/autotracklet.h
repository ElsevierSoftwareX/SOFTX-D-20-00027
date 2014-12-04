#ifndef AUTOTRACKLET_H
#define AUTOTRACKLET_H

#include <QList>
#include <QPair>

#include "../base_data/frame.h"
#include "../base_data/object.h"

class AutoTracklet
{
public:
    AutoTracklet();

private:
    QList<QPair<Frame,Object>> components;
};

#endif // AUTOTRACKLET_H
