#ifndef GENEALOGY_H
#define GENEALOGY_H

#include <memory>

#include <QList>

#include "tracklet.h"
#include "annotation.h"

/*!
 * \brief The Genealogy class
 *
 * This Class represents the generated genealogy. It holds the roots of all the
 * trees generated.
 */
class Genealogy
{
    friend std::ostream& operator<< (std::ostream&, Genealogy&);
public:
    Genealogy();


    std::shared_ptr<Tracklet> getTracklet(int &) const;
    void addTracklet(const std::shared_ptr<Tracklet> &);

private:
    QList<TrackElement> roots;
    QList<std::shared_ptr<Tracklet>> tracklets;
    QList<Annotation> annotations;
};

std::ostream& operator<< (std::ostream&, Genealogy&);

#endif // GENEALOGY_H
