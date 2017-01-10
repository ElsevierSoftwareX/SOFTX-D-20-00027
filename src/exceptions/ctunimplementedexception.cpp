#include "ctunimplementedexception.h"

namespace CellTracker {

CTUnimplementedException::CTUnimplementedException(std::string reason_) :
    reason(reason_) {}

const char *CTUnimplementedException::what() const noexcept
{
    std::string ex = this->CTException::what();
    ex += "Unimplemented: ";
    ex += reason;
    return ex.c_str();
}
}
