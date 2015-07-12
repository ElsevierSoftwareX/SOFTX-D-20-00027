#ifndef CTUNIMPLEMENTEDEXCEPTION_H
#define CTUNIMPLEMENTEDEXCEPTION_H

#include "ctexception.h"

#include <string>

namespace CellTracker {
class CTUnimplementedException : public CTException
{
public:
    CTUnimplementedException(std::string reason);
    ~CTUnimplementedException();

    const char *what() const throw();
private:
    std::string reason;
};
}

#endif // CTUNIMPLEMENTEDEXCEPTION_H
