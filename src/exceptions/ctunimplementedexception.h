#ifndef CTUNIMPLEMENTEDEXCEPTION_H
#define CTUNIMPLEMENTEDEXCEPTION_H

#include "ctexception.h"

#include <string>

namespace CellTracker {

class CTUnimplementedException : public CTException
{
    using CTException::what;

public:
    CTUnimplementedException(std::string reason);

    const char *what() const noexcept;
private:
    std::string reason;
};

}

#endif // CTUNIMPLEMENTEDEXCEPTION_H
