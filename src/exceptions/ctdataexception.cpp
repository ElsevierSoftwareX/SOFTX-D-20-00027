#include "ctdataexception.h"

#include <string>

namespace CellTracker {

CTDataException::CTDataException()
{

}

CTDataException::~CTDataException()
{

}

const char *CTDataException::what() const throw()
{
    std::string ex = this->CTException::what();
    ex += "Data: ";
    return ex.c_str();
}

}
