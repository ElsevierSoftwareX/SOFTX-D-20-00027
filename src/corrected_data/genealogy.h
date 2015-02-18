#ifndef GENEALOGY_H
#define GENEALOGY_H

#include <iostream>
#include <memory>

#include <QList>
#include <QHash>

#include "tracklet.h"
#include "annotation.h"

namespace CellTracker { class Genealogy; }
std::ostream& operator<< (std::ostream&, CellTracker::Genealogy&);

namespace CellTracker {

/*!
 * \brief The Genealogy class
 *
 * This Class represents the generated genealogy. It holds the roots of all the
 * trees generated.
 */
class Genealogy
{
    friend std::ostream& ::operator<< (std::ostream&, Genealogy&);
public:
    Genealogy();

    std::shared_ptr<Tracklet> getTracklet(int &) const;
    void addTracklet(const std::shared_ptr<Tracklet> &);

    std::shared_ptr<QList<Annotation> > getAnnotations() const;
    void setAnnotations(const std::shared_ptr<QList<Annotation> > &value);

private:
    QList<TrackElement> roots;
    QHash<int,std::shared_ptr<Tracklet>> tracklets;
    std::shared_ptr<QList<Annotation>> annotations;
};

}


#endif // GENEALOGY_H
