#include "modifyhdf5.h"

#include <H5Cpp.h>
#include <QString>

#include "hdf5_aux.h"

namespace CellTracker {
bool checkObjectExists(H5::H5File file, std::shared_ptr<Object> object) {
    using namespace H5;

    std::string path = hdfPath(object);
    if (!linkExists(file, path.c_str()))
        return false;

    Group objGroup = file.openGroup(path);
    return (readSingleValue<uint32_t>(objGroup, "frame_id") ==  object->getFrameId()
            && readSingleValue<uint32_t>(objGroup, "slice_id") == object->getSliceId()
            && readSingleValue<uint32_t>(objGroup, "channel_id") == object->getChannelId()
            && readSingleValue<uint32_t>(objGroup, "object_id") == object->getId());
}

bool ModifyHDF5::replaceObject(QString filename, std::shared_ptr<Object> oldObject, std::initializer_list<std::shared_ptr<Object> > newObjects)
{
    using namespace H5;

    H5File file(filename.toStdString().c_str(), H5F_ACC_RDWR);

    if (!checkObjectExists(file, oldObject)) /* old object has to exist */
        return false;
    for (std::shared_ptr<Object> o : newObjects)
        if (checkObjectExists(file, o)) /* new objects may not yet exist */
            return false;

    file.unlink(hdfPath(oldObject));

    for (std::shared_ptr<Object> o : newObjects) {
        openOrCreateGroup(file, hdfPath(o));
    }

    return true;
}
}
