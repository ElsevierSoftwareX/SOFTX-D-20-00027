#ifndef MODIFYHDF5_H
#define MODIFYHDF5_H

#include <memory>
#include <QString>

#include <base/object.h>

namespace CellTracker {
class ModifyHDF5
{
public:
    ModifyHDF5() = delete;
    ~ModifyHDF5() = delete;

    static bool replaceObject(QString filename, std::shared_ptr<Object> oldObject, std::initializer_list<std::shared_ptr<Object>> newObjects);
    static bool replaceObjects(QString filename, std::initializer_list<std::shared_ptr<Object>> oldObjects, std::shared_ptr<Object> newObject);
};
}

#endif // MODIFYHDF5_H
