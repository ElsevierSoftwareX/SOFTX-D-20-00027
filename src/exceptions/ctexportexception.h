#ifndef CTEXPORTEXCEPTION_H
#define CTEXPORTEXCEPTION_H

#include "ctdataexception.h"

#include <string>

namespace CellTracker {

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
