#include "io/hdf5_aux.h"

void saveEmptyGroup() {
    using namespace H5;

    H5File f("/tmp/test.h5", H5F_ACC_RDWR|H5F_ACC_CREAT, H5P_FILE_CREATE);

    Group g = clearOrCreateGroup(f, "/tracklets", 0);

    f.close();
}
