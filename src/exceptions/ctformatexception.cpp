#include "ctformatexception.h"

namespace CellTracker {

CTFormatException::CTFormatException(std::string reason)
{
    this->reason = reason;
}

CTFormatException::~CTFormatException()
{

}

const char *CTFormatException::what() const throw()
{
    std::string ex = this->CTDataException::what();
    ex += "FileFormat: ";
    ex += reason;
    return ex.c_str();
}

}
