#ifndef IMPORTHDF5_H
#define IMPORTHDF5_H

#include "import.h"

#include <memory>

#include <QString>
#include <QImage>
#include <H5Cpp.h>

#include "project.h"


namespace CellTracker {
static double xMyImageHeight;

class ImportHDF5 : public Import
{
public:
    ImportHDF5();
    std::shared_ptr<Project> load(QString);
    std::shared_ptr<QImage> requestImage(QString, int, int, int);

private:
    bool loadInfo(H5::H5File file, std::shared_ptr<Project> proj);
    bool loadAnnotations(H5::H5File file, std::shared_ptr<Project> proj);
    bool loadImages(H5::H5File file, std::shared_ptr<Project> proj);
    bool loadObjects(H5::H5File file, std::shared_ptr<Project> proj);
    bool loadTracklets(H5::H5File file, std::shared_ptr<Project> proj);
    bool loadDaughterRelations(H5::H5File file, std::shared_ptr<Project> proj);

    /* HDF5 callbacks */
    static herr_t process_track_annotations (hid_t group_id, const char *name, void *op_data);
    static herr_t process_object_annotations (hid_t group_id, const char *name, void *op_data);
    static herr_t process_images_frames_slices_channels(hid_t group_id, const char *name, void *op_data);
    static herr_t process_images_frames_slices(hid_t group_id, const char *name, void *op_data);
    static herr_t process_images_frames(hid_t group_id, const char *name, void *op_data);
    static herr_t process_objects_frames_slices_channels_objects_properties(hid_t group_id, const char *name, void *op_data);
    static herr_t process_objects_frames_slices_channels_objects (hid_t group_id, const char *name, void *op_data);
    static herr_t process_objects_frames_slices_channels (hid_t group_id, const char *name, void *op_data);
    static herr_t process_objects_frames_slices (hid_t group_id, const char *name, void *op_data);
    static herr_t process_objects_frames(hid_t group_id, const char *name, void *op_data);
    static herr_t process_tracklets_objects(hid_t group_id, const char *name, void *opdata);
    static herr_t process_tracklets_daughters(hid_t group_id_o, const char *name, void *opdata);
    static herr_t process_tracklets (hid_t group_id, const char *name, void *op_data);

};

}

#endif // IMPORTHDF5_H
