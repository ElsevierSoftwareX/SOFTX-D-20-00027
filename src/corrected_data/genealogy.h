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


    // Annotation-related operations
    std::shared_ptr<QList<std::shared_ptr<Annotation>> > getAnnotations() const;
    void setAnnotations(const std::shared_ptr<QList<std::shared_ptr<Annotation> > > &value);
    std::shared_ptr<Annotation> getAnnotation(std::shared_ptr<Annotateable>);
    void addAnnotation(std::shared_ptr<Annotateable>,std::string);

    // Object-related operations
    std::shared_ptr<Object> getObject(int objId) const;
    void addObject(int trackId, int frameId, int objId);
    int removeObject(int objId);

    // Tracklet-related operations
    std::shared_ptr<Tracklet> getTracklet(int &trackId) const;
    void addTracklet(const std::shared_ptr<Tracklet> &tracklet);
    int removeTracklet(int trackId);

    // TrackEvent-related operations
    bool addDivision(int motherId); /*!< \todo remove, should be checked by addDaughter */
    bool addDaughterTrack(int motherId, int daughterId);

private:
    QList<TrackElement> roots;
    QHash<int,std::shared_ptr<Tracklet>> tracklets;
    std::shared_ptr<QList<std::shared_ptr<Annotation>>> annotations;
};

}


#endif // GENEALOGY_H
