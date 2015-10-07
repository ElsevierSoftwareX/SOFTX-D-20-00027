#include "ctexception.h"

#include <string>

namespace CellTracker {

CTException::CTException() { }

const char *CTException::what() const noexcept
{
    return "CellTracker: ";
}

}
