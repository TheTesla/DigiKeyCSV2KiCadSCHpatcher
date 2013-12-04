#ifndef EXTRA_H_INCLUDED
#define EXTRA_H_INCLUDED

#include <string>

#define COPYRIGHT "Copyright (C) 2013 Stefan Helmert"


enum configstate_et
{
    ERROR = -1,
    NONE = 0,
    INPUT = 1,
    OUTPUT = 2,
    READY = 3,
    EOFile = 255
};

typedef struct ihead_t
{
    std::string iname = "";
    int icol = -1;
    bool namecontains = false;
    bool strcontainsname = false;
} ihead_t;

typedef struct ohead_t
{
    std::string oname = "";
    int ocol = -1;
} ohead_t;

typedef struct iheadvec_t
{
    std::string iFilename = "";
    std::vector<ihead_t> iheadvec;
} iheadvec_t;

typedef struct oheadvec_t
{
    std::string oFilename = "";
    std::vector<ohead_t> oheadvec;
} oheadvec_t;

#endif // EXTRA_H_INCLUDED
