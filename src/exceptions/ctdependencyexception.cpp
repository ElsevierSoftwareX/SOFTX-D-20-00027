#include "ctdependencyexception.h"

#include <iostream>

namespace CellTracker {

CTDependencyException::CTDependencyException(std::string reason_) :
    reason(reason_) {}

const char *CTDependencyException::what() const noexcept
{
    std::string ex = this->CTDataException::what();
    ex += "Dependency: ";
    ex += reason;
    return ex.c_str();
}

}
