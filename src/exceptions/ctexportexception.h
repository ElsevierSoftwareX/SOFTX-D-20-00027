#ifndef CTEXPORTEXCEPTION_H
#define CTEXPORTEXCEPTION_H

#include "ctdataexception.h"

#include <string>

namespace CellTracker {

class CTExportException : public CTDataException
{
public:
    CTExportException(std::string);
    ~CTExportException();

    virtual const char *what() const throw();
private:
    std::string reason;
};

}

#endif // CTEXPORTEXCEPTION_H
