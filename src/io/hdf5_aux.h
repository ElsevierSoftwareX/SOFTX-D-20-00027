#ifndef HDF5_AUX
#define HDF5_AUX

#include <tuple>
#include <list>
#include <H5Cpp.h>

/*!
 * Helper functions for handling HDF5-Files.
 */

/* extension of H5 namespace to allow enabling/disabling of error-printing */
namespace H5 {
void disableErrors();
void enableErrors();
}

/* information about objects in HDF5 */
/*!
 * \brief Tests if a certain Group with name "name" exists in the
 * given Group or File "cfg"
 *
 * \param cfg The Group or File to search in
 * \param name The name of the Group to search
 * \return true if it exists, false if it doesn't
 */
bool groupExists(H5::CommonFG &cfg, const char *name);
/*!
 * \brief Tests if a certain DataSet with name "name" exists in the
 * given Group or File "cfg"
 *
 * \param cfg The Group or File to search in
 * \param name The name of the DataSet to search
 * \return true if it exists, false if it doesn't
 */
bool datasetExists(H5::CommonFG &cfg, const char *name);
/*!
 * \brief Tests if a certain Link (i.e. soft or hard link to Group,
 * Dataset, etc.) with name "name" exists in the given Group or File "cfg"
 *
 * \param cfg The Group or File to search in
 * \param name The name of the Link to search
 * \return true if it exists, false if it doesn't
 */
bool linkExists(H5::CommonFG &cfg, const char *name);
/*!
 * \brief For a given group of name "name" in the Group with GroupID
 * "gid" returns the number of Elements in this Group
 *
 * \param gid HDF5-GroupID that holds the group to inspect
 * \param name name of the group
 * \return number of elements in that group
 */
hsize_t getGroupSize(hid_t gid, const char *name);

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
herr_t add_group_element_name(hid_t group_id, const char *name, void *op_data);
/*!
 * \brief collects all the names of the elements of a group
 * \param cfg Group or File for which the names should be collected
 * \return a std::list of std::string with the names
 */
std::list<std::string> collectGroupElementNames(H5::CommonFG &cfg);

/*!
 * \brief returns the type of the link
 * \param obj the object for which the link type should be returned
 * \return the link type
 */
H5L_type_t getLinkType(H5::H5Object &obj);

/* convenience functions */
/*!
 * \brief opens or creates a DataSet
 * \param cfg where to open or create the DataSet
 * \param name which name to use
 * \param type the DataType to use if it doesn't exist
 * \param space the DataSpace to use if it doesn't exist
 * \return the opened or created DataSet
 */
H5::DataSet openOrCreateDataSet(H5::CommonFG& cfg, const char *name, H5::DataType type, H5::DataSpace space);
/*!
 * \brief opens or creates a Group
 * \param cfg where to open or create the Group
 * \param name which name to use
 * \param size (optional) size hint
 * \return the opened or created Group
 */
H5::Group openOrCreateGroup(H5::CommonFG& cfg, const char *name, int size = 0);
/*!
 * \brief clears or creates a Group
 * \param cfg where to clear or create the Group
 * \param name which name to use
 * \param size (optional) size hint
 * \return the cleared or newly created Group
 */
H5::Group clearOrCreateGroup(H5::CommonFG& cfg, const char *name, int size = 0);
/*!
 * \brief links to a given target. If the link already exists, it will be
 * overwritten
 * \param type type of the link to create
 * \param grp where to create the link
 * \param target the target of the link
 * \param link_name the name of the link
 */
void linkOrOverwriteLink(H5L_type_t type, H5::Group grp, std::string target, std::string link_name);

/* read/write templates */
/*!
 * \brief reads a single value of type T from a given dataset (which should
 * only contain one element)
 *
 * \param dset the DataSet to read from
 * \return the read value
 */
template <typename T> inline T readSingleValue(H5::DataSet dset) {
    T ret;
    H5::DataType dtype = dset.getDataType();

    dset.read(&ret, dtype);

    return ret;
}

/*!
 * \brief wrapper for readSingleValue(H5::DataSet)
 */
template <typename T> inline T readSingleValue(hid_t dset_id) {
    return readSingleValue<T>(H5::DataSet(dset_id));
}

/*!
 * \brief wrapper for readSingleValue(H5::DataSet)
 */
template <typename T> inline T readSingleValue(H5::CommonFG &group, const char *name) {
    T ret;
    H5::DataSet dset = group.openDataSet(name);

    ret = readSingleValue<T>(dset);

    return ret;
}

/*!
 * \brief wrapper for readSingleValue(H5::DataSet)
 */
template <typename T> inline T readSingleValue(hid_t group_id, const char *name) {
    return readSingleValue<T>(H5::Group(group_id), name);
}

/*!
 * \brief reads all values from a multi-dimensional DataSet. It also allocates
 * the needed space. Freeing the allocated space is up to the user of this
 * function
 *
 * \param dset the DataSet to read from
 * \return a tuple (data, size[], rank) where
 * * rank is the rank of the DataSet (i.e. how many dimensions)
 * * size[] is an array of the sizes of the dimensions
 * * data is an array with all the data
 */
template <typename T> inline std::tuple<T *,hsize_t *,int> readMultipleValues(H5::DataSet dset) {
    H5::DataType dtype = dset.getDataType();
    H5::DataSpace dspace = dset.getSpace();

    /* Resize the buffer, so all the Elements fit in */
    int rank = dspace.getSimpleExtentNdims();
    hsize_t *dims = new hsize_t[rank];
    dspace.getSimpleExtentDims(dims);
    T *buf;
    hsize_t size = 1;
    for (int i = 0; i < rank; i++)
        size *= dims[i];
    buf = new T[size];

    dset.read(buf,dtype);

    return std::make_tuple(buf,dims,rank);
}

/*!
 * \brief wrapper for readMultipleValues(H5::DataSet)
 */
template <typename T> inline std::tuple<T *,hsize_t *,int> readMultipleValues(hid_t dset_id) {
    return readMultipleValues<T>(H5::DataSet (dset_id));
}

/*!
 * \brief wrapper for readMultipleValues(H5::DataSet)
 */
template <typename T> inline std::tuple<T *,hsize_t *,int> readMultipleValues(H5::Group group, const char *name) {
    H5::DataSet dset = group.openDataSet(name);

    auto ret = readMultipleValues<T>(dset);

    return ret;
}

/*!
 * \brief wrapper for readMultipleValues(H5::DataSet)
 */
template <typename T> inline std::tuple<T *, hsize_t *, int> readMultipleValues(hid_t group_id, const char *name) {
    return readMultipleValues<T>(H5::Group(group_id), name);
}

/*!
 * \brief writes a single value to a DataSet (which is opened or created)
 *
 * \param value the value to write
 * \param group the Group in which the DataSet will be written
 * \param name the name of the dataset
 * \param type the DataType as which the value should be written
 */
template <typename T>
void writeSingleValue(T value, H5::Group group, const char* name, H5::DataType type) {
    hsize_t dims[] = { 1 };
    H5::DataSpace space(1,dims);
    H5::DataSet set = openOrCreateDataSet(group, name, type, space);
    set.write(&value, type);
}

/*!
 * \brief writes multiple values to a multidimensional DataSet (which is opened
 * or created)
 *
 * \param value the array containing the values
 * \param group the Group in which the DataSet will be written
 * \param name the name of the DataSet
 * \param type the DataType as which the values should be written
 * \param rank the rank of the given array
 * \param dims an array of the dimensions of the given data
 */
template <typename T>
void writeMultipleValues (T *value, H5::Group group, const char* name, H5::DataType type, int rank, hsize_t *dims) {
    H5::DataSpace space(rank, dims);
    H5::DataSet set = openOrCreateDataSet(group, name, type, space);
    set.write(value, type);
}


#endif // HDF5_AUX

