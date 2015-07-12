#ifndef GENEALOGY_H
#define GENEALOGY_H

#include <iostream>
#include <memory>

#include <QList>
#include <QHash>

#include "annotation.h"
#include "base_data/movie.h"
#include "tracklet.h"
#include "project.h"

namespace CellTracker { class Genealogy; class Project; }
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
    Genealogy(std::shared_ptr<Project>);

    // Annotation-related operations
    std::shared_ptr<QList<std::shared_ptr<Annotation>> > getAnnotations() const;
    void setAnnotations(const std::shared_ptr<QList<std::shared_ptr<Annotation> > > &value);
    std::shared_ptr<Annotation> getAnnotation(std::shared_ptr<Annotateable>);
    void addAnnotation(std::shared_ptr<Annotateable>,std::string);

    // Object-related operations
    std::shared_ptr<Object> getObject(int trackId, int frameId, uint32_t objId) const;
    std::shared_ptr<Object> getObjectAt(int frameId, int slice, uint32_t objId) const;
    void addObject(int frameId, int trackId, std::shared_ptr<Object> obj);
    void removeObject(int frameId, int trackId, uint32_t objId);

    // Tracklet-related operations
    std::shared_ptr<Tracklet> getTracklet(int trackId) const;
    std::shared_ptr<QHash<int,std::shared_ptr<Tracklet>>> getTracklets() const;
    bool addTracklet(const std::shared_ptr<Tracklet> &tracklet);
    int removeTracklet(int trackId);

    // Operations regarding adding Objects from AutoTracklets to Tracklets
    bool connectObjects(std::shared_ptr<Object> first, std::shared_ptr<Object> second);
    void allFromAT(std::shared_ptr<Tracklet>, std::shared_ptr<AutoTracklet>);
    void allFromATBetween(std::shared_ptr<Tracklet>, std::shared_ptr<AutoTracklet>, std::shared_ptr<Frame>, std::shared_ptr<Frame>);
    void allFromATFrom(std::shared_ptr<Tracklet>, std::shared_ptr<AutoTracklet>, std::shared_ptr<Frame>);
    void allFromATUntil(std::shared_ptr<Tracklet>, std::shared_ptr<AutoTracklet>, std::shared_ptr<Frame>);

    // TrackEvent-related operations
    bool addDaughterTrack(int motherId, int daughterId) __attribute__ ((deprecated));
    bool setDead(int trackId) __attribute__ ((deprecated));
    bool setLost(int trackId) __attribute__ ((deprecated));
    bool setOpen(int trackId) __attribute__ ((deprecated));
    bool addMerge(int prevId, int mergeId) __attribute__ ((deprecated));
    bool addUnmerge(int mergeId, int nextId) __attribute__ ((deprecated));

    bool addDaughterTrack(std::shared_ptr<Tracklet> mother, std::shared_ptr<Tracklet> daughter);
    bool addDaughterTrack(std::shared_ptr<Tracklet> mother, std::shared_ptr<Object> daughter);
    bool setDead(std::shared_ptr<Tracklet> t);
    bool setLost(std::shared_ptr<Tracklet> t);
    bool setOpen(std::shared_ptr<Tracklet> track);
    bool addMerge(std::shared_ptr<Tracklet> prev, std::shared_ptr<Tracklet> merge);
    bool addUnmerge(std::shared_ptr<Tracklet> merge, std::shared_ptr<Tracklet> next);

private:
    std::shared_ptr<QHash<int,std::shared_ptr<Tracklet>>> tracklets;
    std::shared_ptr<QList<std::shared_ptr<Annotation>>> annotations;
    std::shared_ptr<Project> project;
};

}


#endif // GENEALOGY_H
