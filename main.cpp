#include <iostream>
#include <sstream>
#include "Table.h"
#include <fstream>
#include <string>
#include "extra.h"
#include "configread.h"
#include "tolerance.h"


#define COPYRIGHT "Copyright (C) 2013 Stefan Helmert"

#define SHOW_COPYRIGHT std::cout << std::endl << COPYRIGHT << std::endl << std::endl;

using namespace std;




void updateicol(vector<ihead_t> &iheadvec, Table &itab)
{
    unsigned i, icol;
    string str;
    for(i=0;i<iheadvec.size();i++){
        for(icol=0;icol<itab.getNbrofcols();icol++){
            str = itab.Tableread(0, icol);
            if((iheadvec[i].iname == str) || (iheadvec[i].namecontains && std::string::npos!=str.find(iheadvec[i].iname)) || (iheadvec[i].strcontainsname && (std::string::npos!=iheadvec[i].iname.find(str)))){
                iheadvec[i].icol = icol;
            }
            if('&'==iheadvec[i].iname[0]){
                iheadvec[i].icol = -2;
            }
        }
    }
}

int updateicol(vector<iheadvec_t> &iheadmat, vector<Table*> &itabvec)
{
    unsigned i;
    if(iheadmat.size()>itabvec.size()) return -1;
    for(i=0;i<iheadmat.size();i++){
        updateicol(iheadmat[i].iheadvec, *itabvec[i]);
    }
    return 0;
}

int updateNewTable(vector<ihead_t> &iheadvec, vector<ohead_t> &oheadvec, Table &itab, Table &otab, int orowoff)
{
    unsigned i, row;
    char c;
    std::stringstream valstream;
    if(itab.getNbrofrows()+orowoff>otab.getNbrofrows()) return -1;

    for(i=0;i<min(iheadvec.size(),oheadvec.size());i++){
        // write out tablehead
        c = oheadvec[i].oname[0];
        if('$'==c || '+'==c || '-'==c || '%'==c || '['==c || ']'==c){
            otab.Tablewrite(0, oheadvec[i].ocol, oheadvec[i].oname.substr(1,std::string::npos));
        }else{
            otab.Tablewrite(0, oheadvec[i].ocol, oheadvec[i].oname);
        }
        // process/write table content
        for(row=1;row<itab.getNbrofrows();row++){
            if(1>iheadvec[i].iname.size()) return -4;
            if(-2==iheadvec[i].icol){
                otab.Tablewrite(row + orowoff, oheadvec[i].ocol, iheadvec[i].iname.substr(1, std::string::npos));
            }else if('$'==oheadvec[i].oname[0]){
                otab.Tablewrite(row + orowoff, oheadvec[i].ocol, "");
                valstream.str("");
                valstream << std::scientific << norm_value(itab.Tableread(row, iheadvec[i].icol));
                otab.Tablewrite(row + orowoff, oheadvec[i].ocol, valstream.str());
            }else if('%'==oheadvec[i].oname[0]){
                otab.Tablewrite(row + orowoff, oheadvec[i].ocol, to_string(is_relative(itab.Tableread(row, iheadvec[i].icol))));
            }else if('\\'==oheadvec[i].oname[0]){
                otab.Tablewrite(row + orowoff, oheadvec[i].ocol, firstword(itab.Tableread(row, iheadvec[i].icol)));
            }else if('+'==oheadvec[i].oname[0]){
                otab.Tablewrite(row + orowoff, oheadvec[i].ocol, "");
                valstream.str("");
                valstream << std::scientific << tolupp(itab.Tableread(row, iheadvec[i].icol));
                otab.Tablewrite(row + orowoff, oheadvec[i].ocol, valstream.str());
            }else if('-'==oheadvec[i].oname[0]){
                otab.Tablewrite(row + orowoff, oheadvec[i].ocol, "");
                valstream.str("");
                valstream << std::scientific << tollow(itab.Tableread(row, iheadvec[i].icol));
                otab.Tablewrite(row + orowoff, oheadvec[i].ocol, valstream.str());
            }else if('['==oheadvec[i].oname[0]){
                otab.Tablewrite(row + orowoff, oheadvec[i].ocol, "");
                valstream.str("");
                valstream << std::scientific << vallow(norm_value(itab.Tableread(row, iheadvec[i+1].icol)), tollow(itab.Tableread(row, iheadvec[i].icol)), is_relative(itab.Tableread(row, iheadvec[i].icol)));
                otab.Tablewrite(row + orowoff, oheadvec[i].ocol, valstream.str());
            }else if(']'==oheadvec[i].oname[0]){
                otab.Tablewrite(row + orowoff, oheadvec[i].ocol, "");
                valstream.str("");
                valstream << std::scientific << valupp(norm_value(itab.Tableread(row, iheadvec[i-1].icol)), tolupp(itab.Tableread(row, iheadvec[i].icol)), is_relative(itab.Tableread(row, iheadvec[i].icol)));
                otab.Tablewrite(row + orowoff, oheadvec[i].ocol, valstream.str());
            }else{
                otab.Tablewrite(row + orowoff, oheadvec[i].ocol, itab.Tableread(row, iheadvec[i].icol));
            }
            if(false==otab.OK) return -2;
            if(false==itab.OK) return -3;
        }
    }
    return 0;
}

int updateNewTable(vector<iheadvec_t> &iheadmat, oheadvec_t &oheadvec, vector<Table*> &itabvec, Table &otab)
{
    int orowoff = 0;
    unsigned i;
    if(iheadmat.size()>itabvec.size()) return -1;
    for(i=0;i<iheadmat.size();i++){
        updateNewTable(iheadmat[i].iheadvec, oheadvec.oheadvec, *itabvec[i], otab, orowoff);
        orowoff += itabvec[i]->getNbrofrows() - 1;
    }
    return 0;
}

void ihead_error_reporter(vector<iheadvec_t> &iheadmat)
{
    unsigned i, j;
    for(i=0;i<iheadmat.size();i++){
        for(j=0;j<iheadmat[i].iheadvec.size();j++){
            if(-1==iheadmat[i].iheadvec[j].icol){
                cout << "    ERROR no head entry for \"" << iheadmat[i].iheadvec[j].iname <<"\" on input file" << endl;
            }
        }
    }
}

int doit(string cFilename)
{
    Table *itab_ptr;
    vector<Table*> itabvec;
    Table *otab_ptr;
    ifstream iFile;
    vector<ifstream> iFilevec;
    ofstream oFile;
    oheadvec_t oheadvec;
    vector<iheadvec_t> iheadmat;
    configread conf;
    configstate_et state;
    unsigned row;
    unsigned TotalNbrofrows;
    unsigned TotalNbrofcols;
    unsigned loopround;
    string iFilename, oFilename;
    unsigned i;
    int err;
    SHOW_COPYRIGHT
    err = conf.loadConfig(cFilename.c_str());
    if(0!=err) return 10*err;
    cout << endl << "loaded config file - " << cFilename << endl << endl;
    //looping through multiple runs: inputs to output
    row = 0;
    loopround = 0;
    while(1){
        // reset
        iheadmat.clear();
        oheadvec.oheadvec.clear();
        itabvec.clear();

        TotalNbrofrows = 1;
        TotalNbrofcols = 0;

        state = conf.getConfig(row, iheadmat, oheadvec);
        if(NONE==state || EOFile==state) break;

        cout << "round " << ++loopround << ":" << endl;

        // looping through multiple input files for one output file
        for(i=0;i<iheadmat.size();i++){
            iFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
            try{
                iFile.open(iheadmat[i].iFilename.c_str()); // muss ANSI codiert sein; UTF-8 beginnt mit Steuerzeichen, dass falsch als Zeichen ausgewerte wird
            }
            catch(std::ifstream::failure e){
                return -1;
            }            itab_ptr = new Table;
            cout << "    read input file " << i+1 << " - " << iheadmat[i].iFilename << endl;

            itab_ptr->loadTable(iFile);
            if(iFile.is_open()) iFile.close();
            itab_ptr->rmquotmarks();
            TotalNbrofrows += itab_ptr->getNbrofrows() - 1;
            TotalNbrofcols = max(TotalNbrofcols, itab_ptr->getNbrofcols());
            itabvec.push_back(itab_ptr);

        }
        oFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
        try{
            oFile.open(oheadvec.oFilename.c_str());
        }
        catch(std::ifstream::failure e){
            return -2;
        }
        cout << "    database written to output file - " << oheadvec.oFilename << endl;
        otab_ptr = new Table();
        otab_ptr->newTable(TotalNbrofrows, TotalNbrofcols);
        updateicol(iheadmat, itabvec);
        ihead_error_reporter(iheadmat);
        updateNewTable(iheadmat, oheadvec, itabvec, *otab_ptr);
        err = otab_ptr->saveTable(oFile, "\t");
        delete otab_ptr;
        if(0!=err){
            return 11*err;
        }
        for(i=0;i<iheadmat.size();i++){
            delete itabvec[i];
        }
        if(oFile.is_open()) oFile.close();
    }
    return 0;
}



int main(int argc, char *argv[])
{
    int err;
    if(2==argc){
        err = doit(argv[1]);
        if(-20==err) cerr << "could not open config file - bad" << endl;
        if(-10==err) cerr << "could not open config file" << endl;
        if(-1==err) cerr << "could not open input file" << endl;
        if(-2==err) cerr << "could not open output file" << endl;
        if(-11==err) cerr << "could not write output file" << endl;
        if(-22==err) cerr << "could not write output file - bad" << endl;

    }else{
        cout << "This program is designed to convert csv files from digikey.de to database files for the kicadschpatcher." << endl;
        SHOW_COPYRIGHT
        cout << endl << "usage: digikeycsv2kicadschpatch configfile.conf" << endl;
    }


    return 0;
}
