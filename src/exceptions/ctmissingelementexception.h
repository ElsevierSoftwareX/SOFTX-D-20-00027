#ifndef CTMISSINGELEMENTEXCEPTION_H
#define CTMISSINGELEMENTEXCEPTION_H

#include "ctdataexception.h"

#include <string>

namespace CellTracker {

class CTMissingElementException : public CTDataException
{
public:
    CTMissingElementException(std::string);
    ~CTMissingElementException();

    const char *what() const throw();
private:
    std::string reason;
};

}
#endif // CTMISSINGELEMENTEXCEPTION_H
