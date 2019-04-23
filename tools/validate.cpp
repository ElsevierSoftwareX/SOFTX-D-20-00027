/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2015 Sebastian Wagner
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
    TraCurate::Validator::validTraCurateFile(qs, true, true, true);
    return 0;
}
