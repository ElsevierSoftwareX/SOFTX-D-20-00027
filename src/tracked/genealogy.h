/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2016, 2015 Sebastian Wagner
 *
 * TraCurate is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TraCurate is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with TraCurate.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef GENEALOGY_H
#define GENEALOGY_H

#include <iostream>
#include <memory>

#include <QList>
#include <QHash>

#include "annotation.h"
#include "base/movie.h"
#include "tracklet.h"
#include "project.h"

namespace TraCurate { class Genealogy; class Project; }
std::ostream& operator<< (std::ostream&, TraCurate::Genealogy&);

namespace TraCurate {

/*!
 * \brief The Genealogy class
 *
 * This Class represents the generated genealogy. It holds all the Tracklet%s and Annotation%s
 * and provides operations on those.
 */
class Genealogy
{
    friend std::ostream& ::operator<< (std::ostream&, Genealogy&);
public:
    Genealogy(std::shared_ptr<Project>);

    // Annotation-related operations
    std::shared_ptr<Annotation> getAnnotation(int id) const;
    std::shared_ptr<QList<std::shared_ptr<Annotation>> > getAnnotations() const;
    void setAnnotations(std::shared_ptr<QList<std::shared_ptr<Annotation>>> const &value);
    void addAnnotation(std::shared_ptr<Annotation>);
    void deleteAnnotation(std::shared_ptr<Annotation>);
    void annotate(std::shared_ptr<Annotateable>, std::shared_ptr<Annotation>);
    void unannotate(std::shared_ptr<Annotateable>, std::shared_ptr<Annotation>);
    std::shared_ptr<QList<std::shared_ptr<Annotateable> > > getAnnotated() const;
    void setAnnotated(const std::shared_ptr<QList<std::shared_ptr<Annotateable> > > &value);

    // Object-related operations
    std::shared_ptr<Object> getObject(int trackId, int frameId, uint32_t objId) const;
    std::shared_ptr<Object> getObjectAt(int frameId, int slice, int chanId, uint32_t objId) const;
    void addObject(int frameId, int trackId, std::shared_ptr<Object> const &obj);
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
    bool addDaughterTrack(std::shared_ptr<Tracklet> mother, std::shared_ptr<Object> daughterObj);
    bool addUnmergedTrack(std::shared_ptr<Tracklet> merged, std::shared_ptr<Object> unmergedObj);
    bool addMergedTrack(std::shared_ptr<Tracklet> unmerged, std::shared_ptr<Object> mergedObj);
    bool hasDaughterObject(std::shared_ptr<Tracklet> mother, std::shared_ptr<Object> daughterObj);
    bool hasUnmergerObject(std::shared_ptr<Tracklet> merged, std::shared_ptr<Object> unmergedObj);
    bool hasMergerObject(std::shared_ptr<Tracklet> unmerged, std::shared_ptr<Object> mergedObj);
    bool removeDaughterTrack(std::shared_ptr<Tracklet> mother, std::shared_ptr<Object> daughterObj);
    bool removeUnmergedTrack(std::shared_ptr<Tracklet> merged, std::shared_ptr<Object> unmergedObj);
    bool removeMergedTrack(std::shared_ptr<Tracklet> unmerged, std::shared_ptr<Object> mergedObj);
    bool setDead(std::shared_ptr<Tracklet> t);
    bool setLost(std::shared_ptr<Tracklet> t);
    bool setOpen(std::shared_ptr<Tracklet> track);
    bool addMerge(std::shared_ptr<Tracklet> prev, std::shared_ptr<Tracklet> merge);
    bool addUnmerge(std::shared_ptr<Tracklet> merge, std::shared_ptr<Tracklet> next);

private:
    std::shared_ptr<QHash<int,std::shared_ptr<Tracklet>>> tracklets; /*!< all existing Tracklet%s */
    std::shared_ptr<QList<std::shared_ptr<Annotation>>> annotations; /*!< all existing Annotation%s */
    std::shared_ptr<QList<std::shared_ptr<Annotateable>>> annotated; /*!< all existing Annotateable%s */
    std::weak_ptr<Project> project;                                  /*!< the Project */
};

}

#endif // GENEALOGY_H
