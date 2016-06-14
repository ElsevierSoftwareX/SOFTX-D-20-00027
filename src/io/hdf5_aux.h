#ifndef HDF5_AUX
#define HDF5_AUX

#include <tuple>
#include <list>
#include <memory>
#include <H5Cpp.h>

#include "base/object.h"
#include "base/channel.h"
#include "base/slice.h"
#include "base/frame.h"
#include "base/autotracklet.h"
#include "tracked/tracklet.h"

/*!
 * Helper functions for handling HDF5-Files.
 */

/* extension of H5 namespace to allow enabling/disabling of error-printing */
namespace H5 {
void disableErrors();
void enableErrors();
}

/* information about objects in HDF5 */
bool groupExists(H5::CommonFG &cfg, const char *name);
bool datasetExists(H5::CommonFG &cfg, const char *name);
bool linkExists(H5::CommonFG &cfg, const char *name);
bool linkExists(H5::CommonFG &cfg, std::string name);
hsize_t getGroupSize(hid_t gid, const char *name);

herr_t add_group_element_name(hid_t group_id, const char *name, void *op_data);
std::list<std::string> collectGroupElementNames(H5::CommonFG &cfg);

H5L_type_t getLinkType(H5::H5Object &obj);

/* convenience functions */
H5::DataSet openOrCreateDataSet(H5::CommonFG& cfg, const char *name, H5::DataType type, H5::DataSpace space);
H5::DataSet openOrCreateDataSet(H5::CommonFG& cfg, std::string name, H5::DataType type, H5::DataSpace space);
H5::Group openOrCreateGroup(H5::CommonFG& cfg, const char *name, int size = 0);
H5::Group openOrCreateGroup(H5::CommonFG& cfg, std::string name, int size = 0);
H5::Group clearOrCreateGroup(H5::CommonFG& cfg, const char *name, int size = 0);
H5::Group clearOrCreateGroup(H5::CommonFG& cfg, std::string name, int size = 0);
void linkOrOverwriteLink(H5L_type_t type, H5::Group grp, std::string target, std::string link_name);

herr_t shallowCopy(H5::Group &src, const char *src_name, H5::Group &dst, const char *dst_name);
inline herr_t shallowCopy(H5::Group &src, const char *name, H5::Group &dst) { return shallowCopy(src, name, dst, name); }

typedef bool (*checkFn)(H5::Group &checkee);
struct copy_data {
    H5::Group &dest;
    checkFn check;
};
bool checkAlwaysTrue(H5::Group &g);
herr_t deepCopyCallback(hid_t group_id, const char *name, void *op_data);
void deepConditionalCopy(H5::Group &from, H5::Group &to, checkFn check);

/* read/write templates */
/*!
 * \brief reads a single value of type T from a given dataset (which should
 * only contain one element)
 *
 * \param dset the DataSet to read from
 * \return the read value
 */
template <typename T> inline T readSingleValue(H5::DataSet dset) {
    T ret{};
    H5::DataType dtype = dset.getDataType();

    dset.read(&ret, dtype);

    return ret;
}

/*!
 * \brief wrapper for readSingleValue(H5::DataSet)
 */
template <typename T> inline T readSingleValue(hid_t dset_id) {
    H5::DataSet ds(dset_id);
    return readSingleValue<T>(ds);
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
    H5::Group grp(group_id);
    return readSingleValue<T>(grp, name);
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
    H5::DataSet ds(dset_id);
    return readMultipleValues<T>(ds);
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
    H5::Group grp(group_id);
    return readMultipleValues<T>(grp, name);
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
    H5::DataSpace space(H5S_SCALAR);
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

/*!
 * \brief returns the HDF-Path for a given object.
 * Objects may be shared pointers to:
 * * Object
 * * Channel
 * * Slice
 * * Frame
 * * Tracklet
 * * AutoTracklet
 */
template <typename Obj>
std::string hdfPath(Obj obj);

/*!
 * \brief returns the HDF-Path of a given Object in a Container.
 * Currently implemented options are:
 * * Object in Tracklet
 * * Object in AutoTracklet
 */
template <typename Cont, typename Obj>
std::string hdfPath(Cont cont, Obj obj);

void writeFixedLengthString(std::string value, H5::Group group, const char *name);
std::string readString(H5::Group group, const char *name);

#endif // HDF5_AUX

