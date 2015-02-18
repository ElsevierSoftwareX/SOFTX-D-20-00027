#ifndef CTIOEXCEPTION_H
#define CTIOEXCEPTION_H

#include "ctexception.h"

namespace CellTracker {

class CTDataException : public CTException
{
public:
    CTDataException();
    ~CTDataException();

    virtual const char *what() const throw();
};
}

#endif // CTIOEXCEPTION_H
