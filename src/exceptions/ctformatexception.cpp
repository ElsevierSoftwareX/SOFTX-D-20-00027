#include "ctformatexception.h"

namespace CellTracker {

CTFormatException::CTFormatException(std::string reason) : reason(reason) { }

const char *CTFormatException::what() const noexcept
{
    std::string ex = this->CTDataException::what();
    ex += "FileFormat: ";
    ex += reason;
    return ex.c_str();
}

}
