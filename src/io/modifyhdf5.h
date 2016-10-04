#ifndef MODIFYHDF5_H
#define MODIFYHDF5_H

#include <memory>
#include <QString>

#include <base/object.h>

namespace H5 { class H5File; }

namespace CellTracker {
class ModifyHDF5
{
private:
    static bool removeObject(H5::H5File filename, std::shared_ptr<Object> o);
    static bool insertObject(H5::H5File filename, std::shared_ptr<Object> o);
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
};
}

#endif // MODIFYHDF5_H
