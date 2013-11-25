#ifndef CONFIGREAD_H
#define CONFIGREAD_H

#include <string>
#include <fstream>
#include <vector>
#include "extra.h"
#include "Table.h"

class configread
{
    public:
        configread();
        ~configread();
        int loadConfig(string Filename);
        int loadConfig(ifstream &File);
        //configstate_et getConfig(unsigned &row, vector<headpairs_t> &head2headvec, string &iFilename, string &oFilename);
        configstate_et getConfig(unsigned &row, vector<iheadvec_t> &iheadmat, oheadvec_t &oheadvec);


    protected:
    private:
        Table tab;
};

#endif // CONFIGREAD_H
