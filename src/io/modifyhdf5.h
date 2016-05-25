#ifndef MODIFYHDF5_H
#define MODIFYHDF5_H

#include "modify.h"

#include <memory>
#include <QString>

#include <base/object.h>

namespace CellTracker {
class ModifyHDF5 : public Modify
{
public:
    ModifyHDF5() = default;
    ~ModifyHDF5() = default;

    bool replaceObject(QString filename, std::shared_ptr<Object> oldObject, std::initializer_list<std::shared_ptr<Object> > newObjects);
    bool replaceObjects(QString filename, std::initializer_list<std::shared_ptr<Object> > oldObjects, std::shared_ptr<Object> newObject);
};
}

#endif // MODIFYHDF5_H
