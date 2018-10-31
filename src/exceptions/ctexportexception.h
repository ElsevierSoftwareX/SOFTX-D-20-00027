/*
 * Celltracker – A curation tool for object tracks.
 * Copyright (C) 2015 Sebastian Wagner
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
#ifndef CTEXPORTEXCEPTION_H
#define CTEXPORTEXCEPTION_H

#include "ctdataexception.h"

#include <string>

namespace CellTracker {

/*!
 * \brief The CTExportException class
 *
 * Exception that can be used to signal, that the export to some resource has failed.
 */
class CTExportException : public CTDataException
{
    using CTDataException::what;

public:
    CTExportException(std::string);

    const char *what() const noexcept;
private:
    std::string reason;
};

}

#endif // CTEXPORTEXCEPTION_H
