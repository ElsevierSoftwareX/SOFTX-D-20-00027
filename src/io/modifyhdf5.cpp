/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2017, 2016 Konstantin Thierbach, Sebastian Wagner
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
#include "modifyhdf5.h"

#include <H5Cpp.h>
#include <QString>

#include "hdf5_aux.h"
#include "provider/guistate.h"
#include "provider/messagerelay.h"

namespace TraCurate {
bool ModifyHDF5::checkObjectExists(H5::H5File file, std::shared_ptr<Object> object) {
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

bool ModifyHDF5::checkObjectExistsInTracklet(H5::H5File file, std::shared_ptr<Tracklet> t, std::shared_ptr<Object> o) {
    using namespace H5;

    /*! \todo replace by hdfSearch if groupname is framenumber in data format */
    std::string path = hdfSearch(file, t, o);
    std::string tPath = hdfPath(t);

    if (!linkExists(file, path.c_str()))
        return false;
    if (!linkExists(file, tPath))
        return false;

    Group objGroup = file.openGroup(path);
    Group traGroup = file.openGroup(tPath);

    return (readSingleValue<uint32_t>(objGroup, "frame_id") ==  o->getFrameId()
            && readSingleValue<uint32_t>(objGroup, "slice_id") == o->getSliceId()
            && readSingleValue<uint32_t>(objGroup, "channel_id") == o->getChannelId()
            && readSingleValue<uint32_t>(objGroup, "object_id") == o->getId()
            && readSingleValue<uint32_t>(traGroup, "tracklet_id") == o->getTrackId());
}

bool ModifyHDF5::checkObjectExistsInAutoTracklet(H5::H5File file, std::shared_ptr<AutoTracklet> at, std::shared_ptr<Object> o) {
    using namespace H5;

    /*! \todo replace by hdfSearch if groupname is framenumber in data format */
    std::string path = hdfSearch(file, at, o);
    std::string atPath = hdfPath(at);

    if (!linkExists(file, path.c_str()))
        return false;
    if (!linkExists(file, atPath))
        return false;

    Group objGroup = file.openGroup(path);
    Group atGroup = file.openGroup(atPath);

    return (readSingleValue<uint32_t>(objGroup, "frame_id") ==  o->getFrameId()
            && readSingleValue<uint32_t>(objGroup, "slice_id") == o->getSliceId()
            && readSingleValue<uint32_t>(objGroup, "channel_id") == o->getChannelId()
            && readSingleValue<uint32_t>(objGroup, "object_id") == o->getId()
            && readSingleValue<uint32_t>(atGroup, "autotracklet_id") == o->getAutoId());
}

bool ModifyHDF5::removeObject(H5::H5File file, std::shared_ptr<Object> o) {
    using namespace H5;

    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    if (!o || !proj)
        return false;

    std::string objectPath = hdfPath(o);
    if (!linkExists(file, objectPath)) /* has to exist */
        return false;

    /* remove from tracklet */
    {
        uint32_t tid = o->getTrackId();
        std::shared_ptr<Tracklet> tracklet = proj->getGenealogy()->getTracklet(tid);
        if (tracklet) {
            /* Tracklet might not yet have been written to disk */
            if (checkObjectExistsInTracklet(file, tracklet, o)) {

                /*! \todo replace by hdfSearch if groupname is framenumber in data format */
                std::string objPath = hdfSearch(file, tracklet, o);
                Group objectGroup = file.openGroup(objPath);
                if (getLinkType(objectGroup) == H5L_TYPE_SOFT)
                    file.unlink(objPath);
                /*! \todo update the tracklet (start, end) */
            }
        }
    }
    /* remove from autotracklet */
    {
        uint32_t atid = o->getAutoId();
        std::shared_ptr<AutoTracklet> at = proj->getAutoTracklet(atid);
        if (at) {
            /* These should always exist in the file */
            if (!checkObjectExistsInAutoTracklet(file, at, o))
                return false;

            /*! \todo replace by hdfSearch if groupname is framenumber in data format */
            std::string objPath = hdfSearch(file, at, o);
            Group objectGroup = file.openGroup(objPath);
            if (getLinkType(objectGroup) == H5L_TYPE_SOFT)
                file.unlink(objPath);
            /*! \todo update the autotracklet (start, end) */
        }
    }

    /* remove the object itself */
    Group objectGroup = file.openGroup(objectPath);
    if (getLinkType(objectGroup) == H5L_TYPE_HARD)
        file.unlink(objectPath);
    else
        return false;

    return true;
}

bool ModifyHDF5::removeObject(QString filename, std::shared_ptr<Object> o) {
    using namespace H5;

    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    if (!proj)
        return false;

    if (!H5File::isHdf5(filename.toStdString()))
        return false;
    H5File file(filename.toStdString().c_str(), H5F_ACC_RDWR);

    return removeObject(file, o);
}

bool ModifyHDF5::insertObject(H5::H5File file, std::shared_ptr<Object> o) {
    std::string path = hdfPath(o);
    if (linkExists(file, path)) /* may not yet exist */
        return false;

    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();

    return ExportHDF5::saveObject(file, proj, o);
}

bool ModifyHDF5::insertObject(QString filename, std::shared_ptr<Object> o) {
    using namespace H5;

    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    if (!proj)
        return false;

    if (!H5File::isHdf5(filename.toStdString()))
        return false;
    H5File file(filename.toStdString().c_str(), H5F_ACC_RDWR);

    return insertObject(file, o);
}

bool ModifyHDF5::replaceObjects(QString filename,
                                std::initializer_list<std::shared_ptr<Object>> oldObjects,
                                std::initializer_list<std::shared_ptr<Object>> newObjects)
{
    using namespace H5;

    std::shared_ptr<Project> proj = GUIState::getInstance()->getProj();
    if (!proj)
        return false;

    if (!H5File::isHdf5(filename.toStdString()))
        return false;
    H5File file(filename.toStdString().c_str(), H5F_ACC_RDWR);

    for (std::shared_ptr<Object> o : oldObjects)
        if (!checkObjectExists(file, o)) /* old objects have to exist */
            return false;
    for (std::shared_ptr<Object> o : newObjects)
        if (checkObjectExists(file, o)) /* new objects may not yet exist */
            return false;

    bool ret = true;
    for (std::shared_ptr<Object> o : oldObjects) { /* "delete" the old objects */
        ret &= removeObject(file, o);
        if (!ret) {
            MessageRelay::emitUpdateStatusBar("Could not remove old objects from HDF5 file!");
            return false;
        }
    }
    for (std::shared_ptr<Object> o : newObjects) { /* create the new objects */
        ret &= insertObject(file, o);
        if (!ret) {
            MessageRelay::emitUpdateStatusBar("Could not insert new objects into HDF5 file!");
            return false;
        }
    }

    return ret;
}

bool ModifyHDF5::replaceObjects(QString filename, std::shared_ptr<Object> oldObject, std::initializer_list<std::shared_ptr<Object>> newObjects)
{
    return replaceObjects(filename, {oldObject}, newObjects);
}

bool ModifyHDF5::replaceObjects(QString filename, std::initializer_list<std::shared_ptr<Object>> oldObjects, std::shared_ptr<Object> newObject)
{
    return replaceObjects(filename, oldObjects, {newObject});
}

bool ModifyHDF5::replaceObjects(QString filename, std::shared_ptr<Object> oldObject, std::shared_ptr<Object> newObject)
{
    return replaceObjects(filename, {oldObject}, {newObject});
}
}
