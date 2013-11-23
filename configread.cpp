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
    return tab.loadTable(File, ",", " \t", " \t");
}

configstate_et configread::getConfig(unsigned &row, vector<headpairs_t> &head2headvec, string &iFilename, string &oFilename)
{
    unsigned col;
    string str;
    configstate_et state;
    headpairs_t head2head;
    state = NONE;
    for(;row<tab.getNbrofrows();row++){
            cout << " " << row << endl;
        str = tab.Tableread(row, 0);
        if(false==tab.OK){
            state = EOFile;
            break;
        }
        switch(state){
            case NONE:
                if("INPUT" == str){
                    state = INPUT;
                    iFilename = tab.Tableread(row, 1);
                    break;
                }
                if("OUTPUT" == str){
                    state = ERROR;
                    iFilename = "";
                    oFilename = "";
                    break;
                }
                break;
            case INPUT:
                if("OUTPUT" == str){
                    state = OUTPUT;
                    oFilename = tab.Tableread(row, 1);
                    break;
                }
                for(col=0;col<tab.getNbrofcols();col++){
                    if("#"==tab.Tableread(row,col).substr(0,1)){
                        break;
                    }
                    if("&"==tab.Tableread(row, col).substr(0,1)){
                        head2head.iname = tab.Tableread(row, col);
                    }else{
                        head2head.iname = tab.Tableread(row, col).substr(1, std::string::npos);
                    }
                    head2head.namecontains = ">"==tab.Tableread(row, col).substr(0, 1);
                    head2head.strcontainsname = "<"==tab.Tableread(row, col).substr(0, 1);
                    head2headvec.push_back(head2head);
                }
                break;
            case OUTPUT:
                for(col=0;col<tab.getNbrofcols();col++){
                    if(head2headvec.size()<=col) {
                        state = ERROR;
                        break;
                    }
                    head2headvec[col].ocol = col;
                    head2headvec[col].oname = tab.Tableread(row, col);
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
