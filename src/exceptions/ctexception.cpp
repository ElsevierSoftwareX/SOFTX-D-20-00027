#include "ctexception.h"

#include <string>

namespace CellTracker {

CTException::CTException()
{

}

CTException::~CTException()
{

}

const char *CTException::what() const throw()
{
    return "CellTracker: ";
}

}
