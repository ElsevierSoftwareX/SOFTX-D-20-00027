#include "hdf5_aux.h"
#include <iostream>

namespace H5 {
void *HDF5_ERROR_CLIENT_DATA;
H5E_auto2_t HDF5_ERROR_FUNC;

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

/*!
 * \brief Tests if a certain Group with name "name" exists in the
 * given Group or File "cfg"
 *
 * \param cfg The Group or File to search in
 * \param name The name of the Group to search
 * \return true if it exists, false if it doesn't
 */
bool groupExists(CommonFG &cfg, const char *name) {
    try {
        H5::disableErrors();
        Group group = cfg.openGroup(name);
        H5::enableErrors();
        /* group exists, return true */
        return true;
    } catch (H5::Exception) {
        H5::enableErrors();
        return false;
    }
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
    try {
        H5::disableErrors();
        DataSet ds = cfg.openDataSet(name);
        H5::enableErrors();
        /* dataset exists, return true */
        return true;
    } catch (H5::Exception) {
        H5::enableErrors();
        return false;
    }
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
    htri_t ret = H5Lexists(cfg.getLocId(),name,H5P_DEFAULT);

    if(ret >= 0 && ret == true)
        return true;
    else
        return false;
}

bool linkExists(CommonFG &cfg, std::string name) {
    return linkExists(cfg, name.c_str());
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
Group openOrCreateGroup(CommonFG& cfg, const char *name, int size) {
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
Group clearOrCreateGroup(CommonFG& cfg, const char *name, int size) {
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
    H5std_string name = obj.getObjName();
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

    switch (g.childObjType(name)) {
    case H5O_TYPE_DATASET: { /* Object is a dataset */
        err = shallowCopy(g, name, other);

        if (err < 0) {
            std::cerr << "copying failed" << std::endl;
            return -1;
        } else {
            std::cerr << "copied " << g.getObjName() << "/" << name << std::endl;
        }
        break; }
    case H5O_TYPE_GROUP: { /* Object is a group */
        Group gr = g.openGroup(name);

        err = shallowCopy(g, name, other);

        if (err < 0) {
            std::cerr << "copying failed!" << std::endl;
            return -1;
        } else {
            std::cerr << "copied " << g.getObjName() << "/" << name << std::endl;
        }
        Group grOther = other.openGroup(name);
        std::cerr << other.getObjName() << "/" << name << " contains " << std::endl;
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

DataSet openOrCreateDataSet(CommonFG &cfg, std::string name, DataType type, DataSpace space)
{
    return openOrCreateDataSet(cfg, name.c_str(), type, space);
}

Group openOrCreateGroup(CommonFG &cfg, std::string name, int size)
{
    return openOrCreateGroup(cfg, name.c_str(), size);
}

Group clearOrCreateGroup(CommonFG &cfg, std::string name, int size)
{
    return clearOrCreateGroup(cfg, name.c_str(), size);
}
