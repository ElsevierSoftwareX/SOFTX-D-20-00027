#ifndef CTIOEXCEPTION_H
#define CTIOEXCEPTION_H

#include "ctexception.h"

#include <string>

namespace CellTracker {

/*!
 * \brief The CTDataException class
 *
 * Superclass that groups Exceptions that are related to data.
 */
class CTDataException : public CTException
{
    using CTException::what;

public:
    CTDataException();

    const char *what() const noexcept;
};

}

#endif // CTIOEXCEPTION_H
