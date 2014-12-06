#ifndef AUTOTRACKLET_H
#define AUTOTRACKLET_H

#include <QList>
#include <QPair>

#include "../base_data/frame.h"
#include "../base_data/object.h"

/*!
 * \brief The AutoTracklet class
 *
 * Collects a list of Frame%/Object%-Pairs in an AutoTracklet.
 * These are the automatically generated tracks.
 */
class AutoTracklet
{
public:
    AutoTracklet();

private:
    QList<QPair<Frame,Object>> components;
};

#endif // AUTOTRACKLET_H
