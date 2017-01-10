#ifndef CTUNIMPLEMENTEDEXCEPTION_H
#define CTUNIMPLEMENTEDEXCEPTION_H

#include "ctexception.h"

#include <string>

namespace CellTracker {

/*!
 * \brief The CTUnimplementedException class
 *
 * Exception that can be used to signal, that a code-path was reached that hasn't been implemented yet.
 */
class CTUnimplementedException : public CTException
{
    using CTException::what;

public:
    CTUnimplementedException(std::string);

    const char *what() const noexcept;
private:
    std::string reason;
};

}

#endif // CTUNIMPLEMENTEDEXCEPTION_H
