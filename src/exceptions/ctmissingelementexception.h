#ifndef CTMISSINGELEMENTEXCEPTION_H
#define CTMISSINGELEMENTEXCEPTION_H

#include "ctdataexception.h"

#include <string>

namespace CellTracker {

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
