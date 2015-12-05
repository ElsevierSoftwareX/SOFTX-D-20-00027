#include "io/importhdf5.h"

#include <iostream>

#include <QString>
#include <QFile>

__attribute__((noreturn)) void usage(char *argv[]) {
    std::cerr << "Usage:" << std::endl
              << "\t" << argv[0] << " filename [args]" << std::endl
              << std::endl
              << "filename\tName of the HDF5 file to check" << std::endl;
    exit(-1);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
        usage(argv);

    QString qs(argv[1]);
    QFile qf(qs);
    if (!qf.exists()) {
        std::cerr << qs.toStdString() << " is not a file." << std::endl;
        return -1;
    }
    CellTracker::Validator::validCellTrackerFile(qs, true, true, true);
    return 0;
}
