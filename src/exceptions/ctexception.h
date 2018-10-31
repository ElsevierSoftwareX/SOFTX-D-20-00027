/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2016, 2015 Sebastian Wagner
 *
 * Celltracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Celltracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Celltracker.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef CTEXCEPTION_H
#define CTEXCEPTION_H

#include <exception>

#include <string>

namespace CellTracker {

/*!
 * \brief The CTException class
 *
 * Superclass of the other Exceptions.
 */
class CTException : public std::exception
{
    using std::exception::what;

public:
    CTException() = default;

    const char *what() const noexcept;
};

}

#endif // CTEXCEPTION_H
