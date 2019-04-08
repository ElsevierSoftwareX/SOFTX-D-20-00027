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
#ifndef CTUNIMPLEMENTEDEXCEPTION_H
#define CTUNIMPLEMENTEDEXCEPTION_H

#include "ctexception.h"

#include <string>

namespace TraCurate {

/*!
 * \brief The CTUnimplementedException class
 *
 * Exception that can be used to signal, that a code-path was reached that hasn't been implemented yet.
 */
class CTUnimplementedException : public CTException
{
    using CTException::what;

public:
    CTUnimplementedException(std::string);

    const char *what() const noexcept;
private:
    std::string reason;
};

}

#endif // CTUNIMPLEMENTEDEXCEPTION_H
