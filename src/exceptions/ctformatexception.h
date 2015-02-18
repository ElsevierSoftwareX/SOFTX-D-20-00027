#ifndef CTFORMATEXCEPTION_H
#define CTFORMATEXCEPTION_H

#include "ctdataexception.h"

#include <string>

namespace CellTracker {

class CTFormatException : public CTDataException
{
public:
    CTFormatException(std::string);
    ~CTFormatException();

    const char *what() const throw();

private:
    std::string reason;
};

}

#endif // CTFORMATEXCEPTION_H
