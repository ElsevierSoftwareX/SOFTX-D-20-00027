/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2016, 2015 Sebastian Wagner
 *
 * Celltracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Celltracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Celltracker.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef EXPORTHDF5_H
#define EXPORTHDF5_H

#include "export.h"

#include <memory>

#include <QString>
#include <H5Cpp.h>

#include "hdf5_aux.h"
#include "tracked/trackevent.h"
#include "project.h"

namespace CellTracker {

/*!
 * \brief The ExportHDF5 class
 *
 * This class provides the means to save a project to the disk using the save()-Method.
 * If the target file does not exist, it will be copied from the old file.
 */
class ExportHDF5 : public Export
{
public:
    ExportHDF5() = default;
    ~ExportHDF5() = default;
    bool save(std::shared_ptr<Project>, QString);
    bool save(std::shared_ptr<Project>, QString, SaveOptions &);
    static bool sanityCheckOptions(std::shared_ptr<Project>, QString, SaveOptions &);

    static bool saveObject(H5::H5File file, std::shared_ptr<Project> proj, std::shared_ptr<Object> obj);

private:
    static bool saveObjects(H5::H5File file, std::shared_ptr<Project> proj);
    static bool saveInfo(H5::H5File file, std::shared_ptr<Project> proj);
    static bool saveImages(H5::H5File file, std::shared_ptr<Project> proj);
    static bool saveAutoTracklets(H5::H5File file, std::shared_ptr<Project> proj);
    static bool saveEvents(H5::H5File file, std::shared_ptr<Project> proj);
    static bool saveTracklets(H5::H5File file, std::shared_ptr<Project> project);
    static bool saveAnnotation(H5::Group grp, std::shared_ptr<Annotation> a);
    static bool saveAnnotations(H5::H5File file, std::shared_ptr<Project> project);
    static bool saveTrackletsContained(H5::H5File file, H5::Group grp, std::shared_ptr<Tracklet> t);
    static bool saveTrackletsNextEvent(H5::Group grp, std::shared_ptr<Tracklet> t);
    static bool saveTrackletsPreviousEvent(H5::Group grp, std::shared_ptr<Tracklet> t);

    static bool hasBackingHDF5(std::shared_ptr<Project> const &proj);
    static std::tuple<uint8_t*, hsize_t*, int> imageToBuf(std::shared_ptr<QImage> image);
};

}

#endif // EXPORTHDF5_H
