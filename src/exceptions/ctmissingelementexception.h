#ifndef CTMISSINGELEMENTEXCEPTION_H
#define CTMISSINGELEMENTEXCEPTION_H

#include "ctdataexception.h"

#include <string>

namespace CellTracker {

/*!
 * \brief The CTMissingElementException class
 *
 * Exception that can be used to signal, that a certain Element is missing
 * (e.g. some referenced Group in a HDF5 file).
 */
class CTMissingElementException : public CTDataException
{
    using CTDataException::what;

public:
    CTMissingElementException(std::string);

    const char *what() const noexcept;
private:
    std::string reason;
};

}

#endif // CTMISSINGELEMENTEXCEPTION_H
