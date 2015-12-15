#include "ctexportexception.h"

#include <iostream>

namespace CellTracker {

CTExportException::CTExportException(std::string reason) :
    reason(reason) {}

const char *CTExportException::what() const noexcept
{
    std::string ex = this->CTDataException::what();
    ex += "Export: ";
    ex += reason;
    return ex.c_str();
}

}
