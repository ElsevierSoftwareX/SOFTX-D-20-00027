/*
 * TraCurate – A curation tool for object tracks.
 * Copyright (C) 2017, 2015 Sebastian Wagner
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
#include "tcformatexception.h"

namespace TraCurate {

TCFormatException::TCFormatException(std::string reason_) :
    reason(reason_) {}

const char *TCFormatException::what() const noexcept
{
    std::string ex = this->TCDataException::what();
    ex += "FileFormat: ";
    ex += reason;
    return ex.c_str();
}

}
