#include "configread.h"
#include <iostream>

configread::configread()
{
}

configread::~configread()
{
}


int configread::loadConfig(string Filename)
{
    ifstream File;
    File.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try{
        File.open(Filename.c_str());
    }
    catch(std::ifstream::failure e){
        if(File.bad()) return -2;
        return -1;
    }
    return loadConfig(File);
}

int configread::loadConfig(ifstream &File)
{
    int err;
    err = tab.loadTable(File, ",", " \t", " \t");
    if(0!=err) return err;
    tab.rmquotmarks();
    return 0;
}

configstate_et configread::getConfig(unsigned &row, vector<iheadvec_t> &iheadmat, oheadvec_t &oheadvec)
{
    unsigned col;
    string str;
    configstate_et state;
    headpairs_t head2head;
    ihead_t ihead;
    iheadvec_t iheadvec;
    ohead_t ohead;
    state = NONE;
    for(;row<tab.getNbrofrows();row++){
        str = tab.Tableread(row, 0);
        if(false==tab.OK){
            state = EOFile;
            break;
        }
        switch(state){
            case NONE:
                if("INPUT" == str){
                    state = INPUT;
                    iheadvec.iFilename = tab.Tableread(row, 1);
                    break;
                }
                if("OUTPUT" == str){
                    state = ERROR;
                    iheadvec.iFilename = "";
                    oheadvec.oFilename = "";
                    break;
                }
                break;
            case INPUT:
                if("INPUT" == str){
                    iheadmat.push_back(iheadvec);
                    iheadvec.iheadvec.clear();
                    state = INPUT;
                    iheadvec.iFilename = tab.Tableread(row, 1);
                    break;
                }
                if("OUTPUT" == str){
                    state = OUTPUT;
                    oheadvec.oFilename = tab.Tableread(row, 1);
                    iheadmat.push_back(iheadvec);
                    break;
                }
                for(col=0;col<tab.getNbrofcols();col++){
                    if(1>tab.Tableread(row,col).size()){
                        ihead.iname = "&~";
                        ihead.namecontains = false;
                        ihead.strcontainsname = false;
                        iheadvec.iheadvec.push_back(ihead);
                        continue;
                    }
                    if("#"==tab.Tableread(row,col).substr(0,1)){
                        break;
                    }
                    if("&"==tab.Tableread(row, col).substr(0,1)){
                        ihead.iname = tab.Tableread(row, col);
                    }else{
                        ihead.iname = tab.Tableread(row, col).substr(1, std::string::npos);
                    }
                    ihead.namecontains = ">"==tab.Tableread(row, col).substr(0, 1);
                    ihead.strcontainsname = "<"==tab.Tableread(row, col).substr(0, 1);
                    iheadvec.iheadvec.push_back(ihead);
                }
                break;
            case OUTPUT:
                for(col=0;col<tab.getNbrofcols();col++){
                    ohead.ocol = col;
                    ohead.oname = tab.Tableread(row, col);
                    oheadvec.oheadvec.push_back(ohead);
                }
                state = READY;
                break;
            default:
                break;
        }
        if(EOFile==state || ERROR==state || READY==state) return state;
    }

    return state;
}
