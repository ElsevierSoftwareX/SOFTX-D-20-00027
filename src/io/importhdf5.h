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
#ifndef IMPORTHDF5_H
#define IMPORTHDF5_H

#include "import.h"

#include <memory>

#include <QString>
#include <QImage>
#include <H5Cpp.h>

#include "project.h"


namespace TraCurate {

/*!
 * \brief The ImportHDF5 class
 *
 * The ImportHDF5-class is used to load() a project from a HDF5-file on the
 * disk. It is also used to request individual images via the
 * requestImage()-Method.
 *
 * Also a Validator is implemented here, which can be used to check if a given
 * HDF5-file is a valid TraCurate-project.
 */
class ImportHDF5 : public Import
{
public:
    ImportHDF5() = default;
    ~ImportHDF5() = default;

    std::shared_ptr<Project> load(QString);
    std::shared_ptr<QImage> requestImage(QString, int, int, int);

    static void imageCleanupHandler(void *info);
private:
    static bool loadInfo(H5::H5File file, std::shared_ptr<Project> proj);
    static bool loadEvents(H5::H5File file, std::shared_ptr<Project> proj);
    static bool loadAnnotations(H5::H5File file, std::shared_ptr<Project> proj);
    static bool loadImages(H5::H5File file, std::shared_ptr<Project> proj);
    static bool loadObjects(H5::H5File file, std::shared_ptr<Project> proj);
    static bool loadAutoTracklets(H5::H5File file, std::shared_ptr<Project> proj);
    static bool loadTracklets(H5::H5File file, std::shared_ptr<Project> proj);
    static bool loadEventInstances(H5::H5File file, std::shared_ptr<Project> proj);
    static bool loadAnnotationAssignments(H5::H5File file, std::shared_ptr<Project> proj);

    /* HDF5 callbacks */
    static herr_t process_track_annotations (hid_t group_id, const char *name, void *op_data);
    static herr_t process_object_annotations (hid_t group_id, const char *name, void *op_data);
    static herr_t process_images_frames_slices_channels(hid_t group_id, const char *name, void *op_data);
    static herr_t process_images_frames_slices(hid_t group_id, const char *name, void *op_data);
    static herr_t process_images_frames(hid_t group_id, const char *name, void *op_data);
    static herr_t process_objects_frames_slices_channels_objects (hid_t group_id, const char *name, void *op_data);
    static herr_t process_objects_frames_slices_channels (hid_t group_id, const char *name, void *op_data);
    static herr_t process_objects_frames_slices (hid_t group_id, const char *name, void *op_data);
    static herr_t process_objects_frames(hid_t group_id, const char *name, void *op_data);
    static herr_t process_autotracklets_objects(hid_t group_id, const char *name, void *opdata);
    static herr_t process_autotracklets_events_ids(hid_t group_id, const char *name, void *opdata);
    static herr_t process_autotracklets_events(hid_t group_id_o, const char *name, void *opdata);
    static herr_t process_autotracklets (hid_t group_id, const char *name, void *op_data);
    static herr_t process_tracklets_events_ids(hid_t group_id, const char *name, void *opdata);
    static herr_t process_tracklets_events(hid_t group_id_o, const char *name, void *opdata);
    static herr_t process_tracklets_objects(hid_t group_id, const char *name, void *opdata);
    static herr_t process_tracklets (hid_t group_id, const char *name, void *op_data);

    static std::shared_ptr<QPoint> readCentroid(hid_t objGroup);
    static std::shared_ptr<QRect> readBoundingBox(hid_t objGroup);
    static std::shared_ptr<QPolygonF> readOutline (hid_t objGroup);

    static std::shared_ptr<QImage> bufToImage (uint8_t *buf, hsize_t height, hsize_t width, hsize_t depth);
};

namespace Validator {
enum checkType { TYPE_GROUP, TYPE_DATASET};
struct checkObject;
typedef bool (*checkFun)(H5::H5File, checkObject, std::string, std::string&);
struct checkObject {
    std::string            name;
    bool                   necessary;
    H5L_type_t             link;
    checkType              type;
    checkFun               test;
    std::list<checkObject> dependents;
};
struct workItem {
    std::string prefix;
    checkObject item;
};

bool validTraCurateFile(QString, bool warnType, bool warnLink, bool warnTest);

bool test_groupname_matches_object_id(H5::H5File file, checkObject checkee, std::string prefix, std::string &err);
bool test_groupname_matches_channel_id(H5::H5File file, checkObject checkee, std::string prefix, std::string &err);
bool test_groupname_matches_slice_id(H5::H5File file, checkObject checkee, std::string prefix, std::string &err);
bool test_groupname_matches_frame_id(H5::H5File file, checkObject checkee, std::string prefix, std::string &err);
bool test_groupname_matches_tracklet_id(H5::H5File file, checkObject checkee, std::string prefix, std::string &err);
bool test_groupname_matches_autotracklet_id(H5::H5File file, checkObject checkee, std::string prefix, std::string &err);
}

}




#endif // IMPORTHDF5_H
