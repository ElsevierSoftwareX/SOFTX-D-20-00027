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
    void addComponent(std::shared_ptr<Frame>,std::shared_ptr<Object>);
    void addComponent(QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>);
    void setID(int);

    friend std::ostream& operator<< (std::ostream&, AutoTracklet&);

private:
    int trackID;
    QList<QPair<std::shared_ptr<Frame>,std::shared_ptr<Object>>> components; //
};

std::ostream& operator<< (std::ostream&, AutoTracklet&);
std::ostream& operator<< (std::ostream&, QPair<std::shared_ptr<Frame>, std::shared_ptr<Object>>&);

#endif // AUTOTRACKLET_H
