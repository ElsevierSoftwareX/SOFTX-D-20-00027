#include "hdf5_aux.h"

namespace H5 {
void *HDF5_ERROR_CLIENT_DATA;
H5E_auto2_t HDF5_ERROR_FUNC;

void disableErrors() {
    Exception::getAutoPrint(HDF5_ERROR_FUNC,&HDF5_ERROR_CLIENT_DATA);
    Exception::dontPrint();
}

void enableErrors() {
    Exception::setAutoPrint(HDF5_ERROR_FUNC, HDF5_ERROR_CLIENT_DATA);
}
}

using namespace H5;


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

bool datasetExists(CommonFG &cfg, const char *name) {
    try {
        H5::disableErrors();
        DataSet ds = cfg.openDataSet(name);
        H5::enableErrors();
        /* group exists, return true */
        return true;
    } catch (H5::Exception) {
        H5::enableErrors();
        return false;
    }
}

bool linkExists(CommonFG &cfg, const char *name) {
    htri_t ret = H5Lexists(cfg.getLocId(),name,H5P_DEFAULT);

    if(ret >= 0 && ret == true)
        return true;
    else
        return false;
}

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
 * \brief opens or, if it does not yet exist, creates a H5::Group
 * \param cfg where to place the H5::Group (either H5::H5File or H5::Group)
 * \param name the name of the H5::Group
 * \param size the expected size of the group
 * \return a H5::Group object for the group that was opened or created
 */
Group clearOrCreateGroup(CommonFG& cfg, const char *name, int size) {
    Group group;

    if (groupExists(cfg, name))
        cfg.unlink(name);

    group = cfg.createGroup(name, size);
    return group;
}

void linkOrOverwriteLink(H5L_type_t type, Group grp, std::string target, std::string link_name) {
    if (linkExists(grp, link_name.c_str()))
        grp.unlink(link_name);

    grp.link(type, target, link_name);
}

herr_t add_group_element_name(hid_t group_id __attribute__((unused)), const char *name, void *op_data) {
    std::list<std::string> *list = static_cast<std::list<std::string>*>(op_data);
    std::string _name(name);
    list->push_back(_name);
    return 0;
}

std::list<std::string> collectGroupElementNames(CommonFG &cfg)
{
    std::list<std::string> names;
    cfg.iterateElems(".", NULL, add_group_element_name, &names);
    return names;
}

H5L_type_t getLinkType(H5Object &obj)
{
    H5L_info_t infoBuf;
    H5std_string name = obj.getObjName();
    H5Lget_info(obj.getId(), name.c_str(), &infoBuf, H5P_DEFAULT);
    return infoBuf.type;
}
