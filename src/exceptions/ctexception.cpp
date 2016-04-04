#include "ctexception.h"

#include <string>

namespace CellTracker {

const char *CTException::what() const noexcept
{
    return "CellTracker: ";
}

}
