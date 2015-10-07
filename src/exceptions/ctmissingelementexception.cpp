#include "ctmissingelementexception.h"

namespace CellTracker {

CTMissingElementException::CTMissingElementException(std::string reason) : reason(reason) { }

const char *CTMissingElementException::what() const noexcept
{
    std::string ex = this->CTDataException::what();
    ex += "Missing Element: ";
    ex += reason;
    return ex.c_str();
}

}
