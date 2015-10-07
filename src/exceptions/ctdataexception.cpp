#include "ctdataexception.h"

#include <string>

namespace CellTracker {

CTDataException::CTDataException() { }

const char *CTDataException::what() const noexcept
{
    std::string ex = this->CTException::what();
    ex += "Data: ";
    return ex.c_str();
}

}
