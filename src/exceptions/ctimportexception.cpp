#include "ctimportexception.h"

#include <iostream>

namespace CellTracker {

CTImportException::CTImportException(std::string reason) :
    reason(reason) {}

const char *CTImportException::what() const noexcept
{
    std::string ex = this->CTDataException::what();
    ex += "Import: ";
    ex += reason;
    return ex.c_str();
}

}
