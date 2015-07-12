#include "ctunimplementedexception.h"

namespace CellTracker {

CTUnimplementedException::CTUnimplementedException(std::string reason) : reason(reason) { }

CTUnimplementedException::~CTUnimplementedException() { }

const char *CTUnimplementedException::what() const throw()
{
    std::string ex = this->CTException::what();
    ex += "Unimplemented: ";
    ex += reason;
    return ex.c_str();
}
}
