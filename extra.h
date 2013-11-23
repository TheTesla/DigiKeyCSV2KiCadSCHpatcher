#ifndef EXTRA_H_INCLUDED
#define EXTRA_H_INCLUDED

#include <string>

enum configstate_et
{
    ERROR = -1,
    NONE = 0,
    INPUT = 1,
    OUTPUT = 2,
    READY = 3,
    EOFile = 255
};

typedef struct headpairs_t
{
    std::string iname = "";
    int icol = -1;
    bool namecontains = false;
    bool strcontainsname = false;
    bool takeDatabasefieldname = false;
    std::string oname = "";
    int ocol = -1;
} headpairs_t;

#endif // EXTRA_H_INCLUDED
