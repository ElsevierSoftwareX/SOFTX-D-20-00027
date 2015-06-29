#include "ctexportexception.h"

#include <iostream>

namespace CellTracker {

CTExportException::CTExportException(std::string reason) : reason(reason) { }

CTExportException::~CTExportException()
{

}

const char *CTExportException::what() const throw()
{
    std::string ex = this->CTDataException::what();
    ex += "Export: ";
    ex += reason;
    return ex.c_str();
}

}
