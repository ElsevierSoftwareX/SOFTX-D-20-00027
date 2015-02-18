#ifndef CTIMPORTEXCEPTION_H
#define CTIMPORTEXCEPTION_H

#include "ctdataexception.h"

#include <string>

namespace CellTracker {

class CTImportException : public CTDataException
{
public:
    CTImportException(std::string);
    ~CTImportException();

    virtual const char *what() const throw();
private:
    std::string reason;
};

}

#endif // CTIMPORTEXCEPTION_H
