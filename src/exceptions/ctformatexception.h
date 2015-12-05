#ifndef CTFORMATEXCEPTION_H
#define CTFORMATEXCEPTION_H

#include "ctdataexception.h"

#include <string>

namespace CellTracker {

/*!
 * \brief The CTFormatException class
 *
 * Exception that can be used to signal, that something in the data format is wrong and
 * thus the current action cannot continue.
 */
class CTFormatException : public CTDataException
{
    using CTDataException::what;

public:
    CTFormatException(std::string);

    const char *what() const noexcept;
private:
    std::string reason;
};

}

#endif // CTFORMATEXCEPTION_H
