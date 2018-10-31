/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2017, 2016, 2015 Sebastian Wagner
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
#include "hdf5_aux.h"
#include <iostream>

namespace H5 {
static void *HDF5_ERROR_CLIENT_DATA;
static H5E_auto2_t HDF5_ERROR_FUNC;

/*!
 * \brief disables all HDF5 error-printing
 */
void disableErrors() {
    Exception::getAutoPrint(HDF5_ERROR_FUNC,&HDF5_ERROR_CLIENT_DATA);
    Exception::dontPrint();
}

/*!
 * \brief enables HDF5 error-printing again
 */
void enableErrors() {
    Exception::setAutoPrint(HDF5_ERROR_FUNC, HDF5_ERROR_CLIENT_DATA);
}
}

using namespace H5;

#if H5_VERSION_LE(1,8,12)
H5std_string getObjName(H5Object &obj) {
    H5std_string sname("");

    ssize_t size = H5Iget_name(obj.getId(), NULL, static_cast<size_t>(0));
    char* name = new char[size+1];
    memset(name, 0, size+1);

    H5Iget_name(obj.getId(), name, size+1);
    sname = name;

    delete []name;

    // Return object's name
    return(sname);
}
#endif

#if H5_VERSION_LE(1,8,12)
H5O_type_t childObjType(CommonFG &cfg, const char* objname) {
    H5O_info_t objinfo;
    H5O_type_t objtype = H5O_TYPE_UNKNOWN;

    H5Oget_info_by_name(cfg.getLocId(), objname, &objinfo, H5P_DEFAULT);

    return objtype;
}
#endif


/*!
 * \brief Tests if a certain Group with name "name" exists in the
 * given Group or File "cfg"
 *
 * \param cfg The Group or File to search in
 * \param name The name of the Group to search
 * \return true if it exists, false if it doesn't
 */
bool groupExists(CommonFG &cfg, const char *name) {
    return linkExists(cfg, name) && isGroup(cfg, name);
}

/*!
 * \brief Tests if a certain DataSet with name "name" exists in the
 * given Group or File "cfg"
 *
 * \param cfg The Group or File to search in
 * \param name The name of the DataSet to search
 * \return true if it exists, false if it doesn't
 */
bool datasetExists(CommonFG &cfg, const char *name) {
    return linkExists(cfg, name) && isDataset(cfg, name);
}

/*!
 * \brief Tests if a certain Link (i.e. soft or hard link to Group,
 * Dataset, etc.) with name "name" exists in the given Group or File "cfg"
 *
 * \param cfg The Group or File to search in
 * \param name The name of the Link to search
 * \return true if it exists, false if it doesn't
 */
bool linkExists(CommonFG &cfg, const char *name) {
    /*! \todo: Maybe do this right someday, see #82 */
    H5::disableErrors();
    htri_t tmp = H5Lexists(cfg.getLocId(),name,H5P_DEFAULT);
    bool ret = (tmp >= 0 && tmp == true);

    H5::enableErrors();
    return ret;
}

bool linkExists(CommonFG &cfg, std::string name) {
    return linkExists(cfg, name.c_str());
}

bool linkExists(hid_t gid, const char *name) {
    /*! \todo: Maybe do this right someday, see #82 */
    H5::disableErrors();
    htri_t tmp = H5Lexists(gid, name, H5P_DEFAULT);
    bool ret = (tmp >= 0 && tmp == true);

    H5::enableErrors();
    return ret;
}

bool linkValid(hid_t gid, const char *name) {
    Group grp(gid);
    return linkValid(grp, name);
}

bool linkValid(hid_t gid, std::string name) {
    Group grp(gid);
    return linkValid(grp, name.c_str());
}

bool linkValid(CommonFG &group, const char *name) {
    std::string path = group.getLinkval(name);
    return linkExists(group, path);
}

bool linkValid(CommonFG &group, std::string name) {
    return linkValid(group, name.c_str());
}

bool linkExists(hid_t gid, std::string name) {
    return linkExists(gid, name.c_str());
}

bool isGroup(CommonFG &cfg, const char *name) {
#if H5_VERSION_GE(1,8,13)
    H5O_type_t type = cfg.childObjType(name);
#else
    H5O_type_t type = childObjType(cfg, name);
#endif
    return type == H5O_TYPE_GROUP;
}

bool isDataset(CommonFG &cfg, const char *name) {
#if H5_VERSION_GE(1,8,13)
    H5O_type_t type = cfg.childObjType(name);
#else
    H5O_type_t type = childObjType(cfg, name);
#endif
    return type == H5O_TYPE_DATASET;
}

/*!
 * \brief For a given group of name "name" in the Group with GroupID
 * "gid" returns the number of Elements in this Group
 *
 * \param gid HDF5-GroupID that holds the group to inspect
 * \param name name of the group
 * \return number of elements in that group
 */
hsize_t getGroupSize(hid_t gid, const char *name) {
    H5G_info_t info;
    htri_t ret;

    ret = H5Lexists(gid, name, H5P_DEFAULT);

    if(ret >= 0 && ret == true) {
        hid_t group = H5Gopen(gid,name,H5P_DEFAULT);

        H5Gget_info(group,&info);
        H5Gclose(group);

        return info.nlinks;
    } else
        return 0;

}

/*!
 * \brief opens or, if it does not yet exist, creates a H5::DataSet
 * \param cfg where to place the H5::DataSet (either H5::H5File or H5::Group)
 * \param name the name of the H5::DataSet
 * \param type the H5::DataType of the H5::DataSet
 * \param space the H5::DataSpace of the H5::DataSet
 * \return a H5::DataSet object for the dataset that was opened or created
 */
DataSet openOrCreateDataSet(CommonFG& cfg, const char *name, DataType type, DataSpace space) {
    DataSet ds;

    if (datasetExists(cfg, name))
        ds = cfg.openDataSet(name);
    else
        /* DataSet does not exist, create it */
        ds = cfg.createDataSet(name, type, space);

    return ds;
}

/*!
 * \brief opens or, if it does not yet exist, creates a H5::Group
 * \param cfg where to place the H5::Group (either H5::H5File or H5::Group)
 * \param name the name of the H5::Group
 * \param size the expected size of the group
 * \return a H5::Group object for the group that was opened or created
 */
Group openOrCreateGroup(CommonFG& cfg, const char *name, size_t size) {
    Group group;

    if (groupExists(cfg, name))
        group = cfg.openGroup(name);
    else
        /* Group does not exist, create it */
        group = cfg.createGroup(name, size);

    return group;
}

/*!
 * \brief clears or, if it does not yet exist, creates a H5::Group
 * \param cfg where to clear or create the H5::Group (either H5::H5File or H5::Group)
 * \param name the name of the H5::Group
 * \param size (optional) size hint
 * \return a H5::Group object for the group that was cleared or created
 */
Group clearOrCreateGroup(CommonFG& cfg, const char *name, size_t size) {
    Group group;

    if (groupExists(cfg, name))
        cfg.unlink(name);

    group = cfg.createGroup(name, size);
    return group;
}

/*!
 * \brief links to a given target. If the link already exists, it will be overwritten
 * \param type type of the link to create
 * \param grp where to create the link
 * \param target the target of the link
 * \param link_name the name of the link
 */
void linkOrOverwriteLink(H5L_type_t type, Group grp, std::string target, std::string link_name) {
    if (linkExists(grp, link_name.c_str()))
        grp.unlink(link_name);

    grp.link(type, target, link_name);
}

/*!
 * \brief HDF5-callback that is used by collectGroupElementNames. Adds to the
 * given op_data (which has to be of type std::list<std::string>*) the name of
 * the current element
 *
 * \param group_id the HDF5-GroupID that holds the group to inspect
 * \param name the name of the group to inspect
 * \param op_data a std::list<std::string>* in which the names of the elements
 * are collected
 */
herr_t add_group_element_name(hid_t __attribute__((unused)) group_id, const char *name, void *op_data) {
    std::list<std::string> *list = static_cast<std::list<std::string>*>(op_data);
    std::string _name(name);
    list->push_back(_name);
    return 0;
}

/*!
 * \brief collects all the names of the elements of a group
 * \param cfg Group or File for which the names should be collected
 * \return a std::list of std::string with the names
 */
std::list<std::string> collectGroupElementNames(CommonFG &cfg)
{
    std::list<std::string> names;
    cfg.iterateElems(".", NULL, add_group_element_name, &names);
    return names;
}

/*!
 * \brief returns the type of the link
 * \param obj the object for which the link type should be returned
 * \return the link type
 */
H5L_type_t getLinkType(H5Object &obj)
{
    H5L_info_t infoBuf;
#if H5_VERSION_GE(1,8,13)
    H5std_string name = obj.getObjName();
#else
    H5std_string name = getObjName(obj);
#endif
    H5Lget_info(obj.getId(), name.c_str(), &infoBuf, H5P_DEFAULT);
    return infoBuf.type;
}

bool checkAlwaysTrue(Group &g __attribute__((__unused__))) {
    return true;
}

herr_t deepCopyCallback(hid_t group_id, const char *name, void *op_data) {
    herr_t err = 0;
    Group g(group_id);
    struct copy_data *cd = static_cast<struct copy_data *>(op_data);
    Group other = cd->dest;

#if H5_VERSION_GE(1,8,13)
    switch (g.childObjType(name)) {
#else
    switch (childObjType(g, name)) {
#endif
    case H5O_TYPE_DATASET: { /* Object is a dataset */
        err = shallowCopy(g, name, other);

        if (err < 0) {
            std::cerr << "copying failed" << std::endl;
            return -1;
        } else {
#if H5_VERSION_GE(1,8,13)
            std::cerr << "copied " << g.getObjName() << "/" << name << std::endl;
#else
            std::cerr << "copied " << getObjName(g) << "/" << name << std::endl;
#endif
        }
        break; }
    case H5O_TYPE_GROUP: { /* Object is a group */
        Group gr = g.openGroup(name);

        err = shallowCopy(g, name, other);

        if (err < 0) {
            std::cerr << "copying failed!" << std::endl;
            return -1;
        } else {
#if H5_VERSION_GE(1,8,13)
            std::cerr << "copied " << g.getObjName() << "/" << name << std::endl;
#else
            std::cerr << "copied " << getObjName(g) << "/" << name << std::endl;
#endif
        }
        Group grOther = other.openGroup(name);
#if H5_VERSION_GE(1,8,13)
        std::cerr << other.getObjName() << "/" << name << " contains " << std::endl;
#else
        std::cerr << getObjName(other) << "/" << name << " contains " << std::endl;
#endif
        for (std::string s : collectGroupElementNames(grOther))
            std::cerr << " " << s << std::endl;
        deepConditionalCopy(gr, grOther, cd->check);
        break; }
    case H5O_TYPE_UNKNOWN:
    case H5O_TYPE_NAMED_DATATYPE:
    case H5O_TYPE_NTYPES:
        /* error, unhandled */
        err = -1;
        std::cerr << "unhandled object type" << std::endl;
        break;
    }

    return err;
}

void deepConditionalCopy(Group &from, Group &to, checkFn check)
{
    if (check(from)) {
        struct copy_data d = {to, check};
        from.iterateElems(".", NULL, deepCopyCallback, &d);
    }
}

herr_t shallowCopy(Group &src, const char *src_name, Group &dst, const char *dst_name)
{
    herr_t err = 0;
    hid_t ocpypl_id = H5Pcreate(H5P_OBJECT_COPY);
    /*! \todo even with H5O_COPY_SHALLOW_HIERARCHY_FLAG, this still copies first level children of groups */
    H5Pset_copy_object(ocpypl_id, H5O_COPY_SHALLOW_HIERARCHY_FLAG);
    hid_t lcpl_id = H5P_LINK_CREATE_DEFAULT;

    err = H5Ocopy(src.getId(), src_name, dst.getId(), dst_name, ocpypl_id, lcpl_id);

    /*! \todo for debugging purposes, remove later */
    dst.flush(H5F_SCOPE_GLOBAL);

    H5Pclose(ocpypl_id);
    return err;
}

template <>
std::string hdfPath(std::shared_ptr<CellTracker::Object> obj) {
    return "/objects/frames/" + std::to_string(obj->getFrameId())
            + "/slices/" + std::to_string(obj->getSliceId())
            + "/channels/" + std::to_string(obj->getChannelId())
            + "/objects/" + std::to_string(obj->getId());
}

template <>
std::string hdfPath(std::shared_ptr<CellTracker::Channel> channel) {
    return "/objects/frames/" + std::to_string(channel->getFrameId())
            + "/slices/" + std::to_string(channel->getSliceId())
            + "/channels/" + std::to_string(channel->getChanId());
}

template <>
std::string hdfPath(std::shared_ptr<CellTracker::Slice> slice) {
    return "/objects/frames/" + std::to_string(slice->getFrameId())
            + "/slices/" + std::to_string(slice->getSliceId());
}

template <>
std::string hdfPath(std::shared_ptr<CellTracker::Frame> frame) {
    return "/objects/frames/" + std::to_string(frame->getID());
}

template <>
std::string hdfPath(std::shared_ptr<CellTracker::AutoTracklet> at) {
    return "/autotracklets/" + std::to_string(at->getID());
}

template <>
std::string hdfPath(std::shared_ptr<CellTracker::Tracklet> t) {
    return "/tracklets/" + std::to_string(t->getId());
}

template <>
std::string hdfPath(std::shared_ptr<CellTracker::Tracklet> tracklet, std::shared_ptr<CellTracker::Object> obj)
{
    return hdfPath(tracklet) + "/objects/" + std::to_string(obj->getFrameId());
}

template <>
std::string hdfPath(std::shared_ptr<CellTracker::AutoTracklet> at, std::shared_ptr<CellTracker::Object> obj)
{
    return hdfPath(at) + "/objects/" + std::to_string(obj->getFrameId());
}

template <>
std::string hdfSearch(H5::H5File file, std::shared_ptr<CellTracker::Tracklet> tracklet, std::shared_ptr<CellTracker::Object> obj)
{
    using namespace H5;
    std::string trackletPath = hdfPath(tracklet);

    if (linkExists(file, trackletPath)) {
        Group trackletGroup = file.openGroup(trackletPath);
        Group objectsGroup = trackletGroup.openGroup("objects");

        std::list<std::string> names = collectGroupElementNames(objectsGroup);
        for (std::string &name : names) {
            std::string currPath = trackletPath + "/objects/" + name;
            if (isObject(file, currPath, obj))
                return currPath;
        }
    }

    return "";
}

template <>
std::string hdfSearch(H5::H5File file, std::shared_ptr<CellTracker::AutoTracklet> at, std::shared_ptr<CellTracker::Object> obj)
{
    using namespace H5;
    std::string atPath = hdfPath(at);

    Group atGroup = file.openGroup(atPath);
    Group objectsGroup = atGroup.openGroup("objects");

    std::list<std::string> names = collectGroupElementNames(objectsGroup);
    for (std::string &name : names) {
        std::string currPath = atPath + "/objects/" + name;
        if (isObject(file, currPath, obj))
            return currPath;
    }

    return "";
}

bool isObject(H5::H5File file, std::string &path, std::shared_ptr<CellTracker::Object> object) {
    using namespace H5;

    Group objGroup = file.openGroup(path);
    return readSingleValue<uint32_t>(objGroup, "frame_id") ==  object->getFrameId()
            && readSingleValue<uint32_t>(objGroup, "slice_id") == object->getSliceId()
            && readSingleValue<uint32_t>(objGroup, "channel_id") == object->getChannelId()
            && readSingleValue<uint32_t>(objGroup, "object_id") == object->getId();
}

DataSet openOrCreateDataSet(CommonFG &cfg, std::string name, DataType type, DataSpace space)
{
    return openOrCreateDataSet(cfg, name.c_str(), type, space);
}

Group openOrCreateGroup(CommonFG &cfg, std::string name, size_t size)
{
    return openOrCreateGroup(cfg, name.c_str(), size);
}

Group clearOrCreateGroup(CommonFG &cfg, std::string name, size_t size)
{
    return clearOrCreateGroup(cfg, name.c_str(), size);
}

void writeFixedLengthString(std::string value, H5::CommonFG &group, const char *name) {
    H5::StrType st(H5::PredType::C_S1);
    st.setSize(value.length());
    st.setStrpad(H5T_STR_NULLPAD);
    st.setCset(H5T_CSET_ASCII);
    H5::DataSpace space(H5S_SCALAR);
    H5::DataSet set = openOrCreateDataSet(group, name, st, space);
    set.write(value, st);
}

void writeFixedLengthString(const char *value, H5::CommonFG &group, const char *name) {
    std::string s(value);
    writeFixedLengthString(s, group, name);
}

std::string readString(Group group, const char *name)
{
    H5std_string ret;
    H5::DataSet dset = group.openDataSet(name);
    H5::StrType dtype = dset.getStrType();

    dset.read(ret, dtype);

    return ret;
}
