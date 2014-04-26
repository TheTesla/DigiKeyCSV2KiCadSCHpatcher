/* Zusatzfunktionen und -Strukturen, die in fast allen Komponenten gebraucht werden
 *
 * Copyright (C) 2013 Stefan Helmert <stefan.helmert@gmx.net>
 *
 *
 *

 */

#ifndef EXTRAOPS_H_INCLUDED
#define EXTRAOPS_H_INCLUDED

#include <vector>
#include <string>
#include <cmath>
#include <stdexcept>      // std::out_of_range

using namespace std;

typedef struct datapair_t
{
    string fieldname;
    string fieldentry;
    bool namecontains = false; // fieldname ist in Tabelleneintrag enthalten
    bool entrycontains = false;  // fieldentry ist in Tabelleneintrag enthalten
    bool strcontainsname = false; // Tabelleneintrag ist in fieldname enthalten
    bool strcontainsentry = false; // Tabelleneintrag ist in fieldentry enthalten
    bool takeDatabasefieldname = false; // Vollstaendigen fieldname aus Datenbank in SCH-file uebernehmen, ansonsten Suchwort aus config-file
    bool overwrite = false;
    bool resetparams = false;
    bool allowemptyentries = false;
} datapair_t;

typedef struct CSVparams_t
{
    string filename = "Database.csv";
    string delim = "\t";
    string ignorebefore = " ";
    string ignoreafter = " ";
} CSVparams_t;

string rmquotmarks(string str);
void rmquotmarks(vector<datapair_t> &data);
double norm_value(string str, size_t &start);
double norm_value(string str);

#endif // EXTRAOPS_H_INCLUDED
