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
#ifndef TCMISSINGELEMENTEXCEPTION_H
#define TCMISSINGELEMENTEXCEPTION_H

#include "tcdataexception.h"

#include <string>

namespace TraCurate {

/*!
 * \brief The TCMissingElementException class
 *
 * Exception that can be used to signal, that a certain Element is missing
 * (e.g. some referenced Group in a HDF5 file).
 */
class TCMissingElementException : public TCDataException
{
    using TCDataException::what;

public:
    TCMissingElementException(std::string);

    const char *what() const noexcept;
private:
    std::string reason;
};

}

#endif // TCMISSINGELEMENTEXCEPTION_H
