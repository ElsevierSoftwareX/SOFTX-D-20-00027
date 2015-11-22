#ifndef HDF5_AUX
#define HDF5_AUX

#include <tuple>
#include <list>
#include <H5Cpp.h>



/* extension of H5 namespace to allow enabling/disabling of error-printing */
namespace H5 {
void disableErrors();
void enableErrors();
}

/* information about objects in HDF5 */
bool groupExists(H5::CommonFG &cfg, const char *name);
bool datasetExists(H5::CommonFG &cfg, const char *name);
bool linkExists(H5::CommonFG &cfg, const char *name);
hsize_t getGroupSize(hid_t gid, const char *name);

herr_t add_group_element_name(hid_t group_id, const char *name, void *op_data);
std::list<std::string> collectGroupElementNames(H5::CommonFG &cfg);

/* convenience functions */
H5::DataSet openOrCreateDataSet(H5::CommonFG& cfg, const char *name, H5::DataType type, H5::DataSpace space);
H5::Group openOrCreateGroup(H5::CommonFG& cfg, const char *name, int size = 0);
H5::Group clearOrCreateGroup(H5::CommonFG& cfg, const char *name, int size = 0);
void linkOrOverwriteLink(H5L_type_t type, H5::Group grp, std::string target, std::string link_name);

/* read/write templates */
template <typename T> inline T readSingleValue(H5::DataSet dset) {
    T ret;
    H5::DataType dtype = dset.getDataType();

    dset.read(&ret, dtype);

    return ret;
}

template <typename T> inline T readSingleValue(hid_t dset_id) {
    return readSingleValue<T>(H5::DataSet(dset_id));
}

template <typename T> inline T readSingleValue(H5::CommonFG &group, const char *name) {
    T ret;
    H5::DataSet dset = group.openDataSet(name);

    ret = readSingleValue<T>(dset);

    return ret;
}

template <typename T> inline T readSingleValue(hid_t group_id, const char *name) {
    return readSingleValue<T>(H5::Group(group_id), name);
}

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

template <typename T> inline std::tuple<T *,hsize_t *,int> readMultipleValues(hid_t dset_id) {
    return readMultipleValues<T>(H5::DataSet (dset_id));
}

template <typename T> inline std::tuple<T *,hsize_t *,int> readMultipleValues(H5::Group group, const char *name) {
    H5::DataSet dset = group.openDataSet(name);

    auto ret = readMultipleValues<T>(dset);

    return ret;
}

template <typename T> inline std::tuple<T *, hsize_t *, int> readMultipleValues(hid_t group_id, const char *name) {
    return readMultipleValues<T>(H5::Group(group_id), name);
}

template <typename T>
void writeSingleValue(T value, H5::Group group, const char* name, H5::DataType type) {
    hsize_t dims[] = { 1 };
    H5::DataSpace space(1,dims);
    H5::DataSet set = openOrCreateDataSet(group, name, type, space);
    set.write(&value, type);
}

template <typename T>
void writeMultipleValues (T *value, H5::Group group, const char* name, H5::DataType type, int rank, hsize_t *dims) {
    H5::DataSpace space(rank, dims);
    H5::DataSet set = openOrCreateDataSet(group, name, type, space);
    set.write(value, type);
}


#endif // HDF5_AUX

