#ifndef CTIMPORTEXCEPTION_H
#define CTIMPORTEXCEPTION_H

#include "ctdataexception.h"

#include <string>

namespace CellTracker {

/*!
 * \brief The CTDependencyException class
 *
 * Exception that can be used to signal, that some dependency was unmet.
 */
class CTDependencyException : public CTDataException
{
    using CTDataException::what;

public:
    CTDependencyException(std::string);

    const char *what() const noexcept;
private:
    std::string reason;
};

}

#endif // CTIMPORTEXCEPTION_H
