#include "ctmissingelementexception.h"

namespace CellTracker {

CTMissingElementException::CTMissingElementException(std::string reason) : reason(reason) { }

CTMissingElementException::~CTMissingElementException()
{

}

const char *CTMissingElementException::what() const throw()
{
    std::string ex = this->CTDataException::what();
    ex += "Missing Element: ";
    ex += reason;
    return ex.c_str();
}

}
