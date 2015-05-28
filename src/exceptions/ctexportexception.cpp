#include "ctexportexception.h"

#include <iostream>

namespace CellTracker {

CTExportException::CTExportException(std::string reason)
{
    this->reason = reason;
}

CTExportException::~CTExportException()
{

}

const char *CTExportException::what() const throw()
{
//    return "CTExportException";
    std::string ex = this->CTDataException::what();
    ex += "Export: ";
    ex += reason;
    return ex.c_str();
}

}
