#include "ctimportexception.h"

#include <iostream>

namespace CellTracker {

CTImportException::CTImportException(std::string reason)
{
    this->reason = reason;
}

CTImportException::~CTImportException()
{

}

const char *CTImportException::what() const throw()
{
//    return "CTImportException";
    std::string ex = this->CTDataException::what();
    ex += "Import: ";
    ex += reason;
    return ex.c_str();
}

}
