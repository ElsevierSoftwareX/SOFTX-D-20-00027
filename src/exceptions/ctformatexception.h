#ifndef CTFORMATEXCEPTION_H
#define CTFORMATEXCEPTION_H

#include "ctdataexception.h"

#include <string>

namespace CellTracker {

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
