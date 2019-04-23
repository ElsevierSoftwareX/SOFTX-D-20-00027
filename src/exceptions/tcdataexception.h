/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2016, 2015 Sebastian Wagner
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
#ifndef TCIOEXCEPTION_H
#define TCIOEXCEPTION_H

#include "tcexception.h"

#include <string>

namespace TraCurate {

/*!
 * \brief The TCDataException class
 *
 * Superclass that groups Exceptions that are related to data.
 */
class TCDataException : public TCException
{
    using TCException::what;

public:
    TCDataException() = default;

    const char *what() const noexcept;
};

}

#endif // TCIOEXCEPTION_H
