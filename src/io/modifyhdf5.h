/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2016 Konstantin Thierbach, Sebastian Wagner
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
