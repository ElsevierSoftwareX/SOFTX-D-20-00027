#ifndef CTIMPORTEXCEPTION_H
#define CTIMPORTEXCEPTION_H

#include "ctdataexception.h"

#include <string>

namespace CellTracker {

/*!
 * \brief The CTImportException class
 *
 * Exception that can be used to signal, that the import from some resource has failed.
 */
class CTImportException : public CTDataException
{
    using CTDataException::what;

public:
    CTImportException(std::string);

    const char *what() const noexcept;
private:
    std::string reason;
};

}

#endif // CTIMPORTEXCEPTION_H
