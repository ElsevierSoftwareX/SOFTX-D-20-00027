#ifndef MODIFYHDF5_H
#define MODIFYHDF5_H

#include <memory>
#include <QString>
#include <initializer_list>

#include <base/autotracklet.h>
#include <base/object.h>
#include <tracked/tracklet.h>

namespace H5 { class H5File; }

namespace CellTracker {
class ModifyHDF5
{
public:
    ModifyHDF5() = delete;
    ~ModifyHDF5() = delete;

    static bool removeObject(QString filename, std::shared_ptr<Object> o);
    static bool insertObject(QString filename, std::shared_ptr<Object> o);

    static bool replaceObjects(QString filename,
                               std::initializer_list<std::shared_ptr<Object> > oldObjects,
                               std::initializer_list<std::shared_ptr<Object> > newObjects);
    static bool replaceObjects(QString filename,
                              std::shared_ptr<Object> oldObject,
                              std::initializer_list<std::shared_ptr<Object>> newObjects);
    static bool replaceObjects(QString filename,
                               std::initializer_list<std::shared_ptr<Object>> oldObjects,
                               std::shared_ptr<Object> newObject);
    static bool replaceObjects(QString filename,
                               std::shared_ptr<Object> oldObject,
                               std::shared_ptr<Object> newObject);

private:
    static bool removeObject(H5::H5File filename, std::shared_ptr<Object> o);
    static bool insertObject(H5::H5File filename, std::shared_ptr<Object> o);
    static bool checkObjectExists(H5::H5File file, std::shared_ptr<Object> object);
    static bool checkObjectExistsInTracklet(H5::H5File file, std::shared_ptr<Tracklet> t, std::shared_ptr<Object> o);
    static bool checkObjectExistsInAutoTracklet(H5::H5File file, std::shared_ptr<AutoTracklet> at, std::shared_ptr<Object> o);
};
}

#endif // MODIFYHDF5_H
