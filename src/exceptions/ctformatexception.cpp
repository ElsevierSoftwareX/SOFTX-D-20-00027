#include "ctformatexception.h"

namespace CellTracker {

CTFormatException::CTFormatException(std::string reason_) :
    reason(reason_) {}

const char *CTFormatException::what() const noexcept
{
    std::string ex = this->CTDataException::what();
    ex += "FileFormat: ";
    ex += reason;
    return ex.c_str();
}

}
