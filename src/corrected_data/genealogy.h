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
    void addObject(int frameId, int trackId, std::shared_ptr<Object> obj);
    void removeObject(int frameId, int trackId, uint32_t objId);

    // Tracklet-related operations
    std::shared_ptr<Tracklet> getTracklet(int trackId) const;
    bool addTracklet(const std::shared_ptr<Tracklet> &tracklet);
    int removeTracklet(int trackId);

    void connectObjects(std::shared_ptr<Object> first, std::shared_ptr<Object> second);

    // TrackEvent-related operations
    bool addDaughterTrack(int motherId, int daughterId);
    bool setDead(int trackId);
    bool setLost(int trackId);
    bool setOpen(int trackId);
    bool addMerge(int prevId, int mergeId);
    bool addUnmerge(int mergeId, int nextId);

private:
    QHash<int,std::shared_ptr<Tracklet>> tracklets;
    std::shared_ptr<QList<std::shared_ptr<Annotation>>> annotations;
    std::shared_ptr<Movie> movie;
    std::shared_ptr<Project> project;
};

}


#endif // GENEALOGY_H
