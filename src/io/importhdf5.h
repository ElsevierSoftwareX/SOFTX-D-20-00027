#ifndef IMPORTHDF5_H
#define IMPORTHDF5_H

#include "import.h"

#include <memory>

#include <QString>
#include <QImage>
#include <H5Cpp.h>

#include "project.h"


namespace CellTracker {

class ImportHDF5 : public Import
{
public:
    ImportHDF5();
    ~ImportHDF5();

    std::shared_ptr<Project> load(QString);
    std::shared_ptr<QImage> requestImage(QString, int, int, int);


private:
    static bool loadInfo(H5::H5File file, std::shared_ptr<Project> proj);
    static bool loadAnnotations(H5::H5File file, std::shared_ptr<Project> proj);
    static bool loadImages(H5::H5File file, std::shared_ptr<Project> proj);
    static bool loadObjects(H5::H5File file, std::shared_ptr<Project> proj);
    static bool loadAutoTracklets(H5::H5File file, std::shared_ptr<Project> proj);
    static bool loadTracklets(H5::H5File file, std::shared_ptr<Project> proj);
    static bool loadDaughterRelations(H5::H5File file, std::shared_ptr<Project> proj);

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
    static herr_t process_autotracklets_objects(hid_t group_id, const char *name, void *opdata);
    static herr_t process_autotracklets_daughters(hid_t group_id_o, const char *name, void *opdata);
    static herr_t process_autotracklets (hid_t group_id, const char *name, void *op_data);

    static std::shared_ptr<QImage> bufToImage (uint8_t *buf, hsize_t height, hsize_t width, hsize_t depth);
    static std::shared_ptr<QPoint> readCentroid(hid_t objGroup);
    static std::shared_ptr<QRect> readBoundingBox(hid_t objGroup);
    static std::shared_ptr<QPolygonF> readOutline (hid_t objGroup);

};

}

#endif // IMPORTHDF5_H
