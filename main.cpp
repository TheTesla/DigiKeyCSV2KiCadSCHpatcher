#include <iostream>
#include "Table.h"
#include <fstream>
#include <string>
#include "extra.h"
#include "configread.h"

using namespace std;




int updateicol(vector<ihead_t> &iheadvec, Table &itab)
{
    unsigned i, icol;
    string str;
    for(i=0;i<iheadvec.size();i++){
        for(icol=0;icol<itab.getNbrofcols();icol++){
            cout << "updateicol: " << icol << " " << i << endl;
            str = itab.Tableread(0, icol);
            cout << str << endl;
            if((iheadvec[i].iname == str) || (iheadvec[i].namecontains && std::string::npos!=str.find(iheadvec[i].iname)) || (iheadvec[i].strcontainsname && (std::string::npos!=iheadvec[i].iname.find(str)))){
                iheadvec[i].icol = icol;
            }
        }
    }
    return 0;
}

int updateicol(vector<iheadvec_t> &iheadmat, vector<Table*> &itabvec)
{
    unsigned i;
    //cout << "size: " << iheadmat.size() << " " << itabvec.size() << endl;
    for(i=0;i<iheadmat.size();i++){
        updateicol(iheadmat[i].iheadvec, *itabvec[i]);
    }

    return 0;
}

int updateNewTable(vector<ihead_t> &iheadvec, vector<ohead_t> &oheadvec, Table &itab, Table &otab, int orowoff)
{
    unsigned i, row;
    for(i=0;i<iheadvec.size();i++){
        otab.Tablewrite(0, oheadvec[i].ocol, oheadvec[i].oname);
        for(row=1;row<itab.getNbrofrows();row++){
            if("&"==iheadvec[i].iname.substr(0,1)){
                otab.Tablewrite(row + orowoff, oheadvec[i].ocol, iheadvec[i].iname.substr(1,std::string::npos));
            }else{
                otab.Tablewrite(row + orowoff, oheadvec[i].ocol, itab.Tableread(row, iheadvec[i].icol));
            }
        }
    }
    return 0;
}

int updateNewTable(vector<iheadvec_t> &iheadmat, oheadvec_t &oheadvec, vector<Table*> &itabvec, Table &otab)
{
    int orowoff = 0;
    int i;
    for(i=0;i<iheadmat.size();i++){
        updateNewTable(iheadmat[i].iheadvec, oheadvec.oheadvec, *itabvec[i], otab, orowoff);
        orowoff += itabvec[i]->getNbrofrows() - 1;
    }
    return 0;
}

int main()
{
    Table *itab_ptr;
    vector<Table*> itabvec;
    Table otab;
    ifstream iFile;
    vector<ifstream> iFilevec;
    ofstream oFile;
    vector<headpairs_t> head2headvec;
    headpairs_t head2head;
    oheadvec_t oheadvec;
    vector<iheadvec_t> iheadmat;
    configread conf;
    configstate_et state;
    unsigned row;
    unsigned TotalNbrofrows;
    unsigned TotalNbrofcols;
    string iFilename, oFilename;
    int i;
    conf.loadConfig("digikey2kicadschpatcher.conf");
    state = NONE;
    TotalNbrofrows = 0;
    TotalNbrofcols = 0;
    while(1){
        head2headvec.clear();
        state = conf.getConfig(row, iheadmat, oheadvec);
        if(NONE==state || EOFile==state) break;
        for(i=0;i<iheadmat.size();i++){
            iFile.open(iheadmat[i].iFilename.c_str()); // muss ANSI codiert sein; UTF-8 beginnt mit Steuerzeichen, dass falsch als Zeichen ausgewerte wird
            itab_ptr = new Table;
            itab_ptr->loadTable(iFile);
            iFile.close();

            itab_ptr->rmquotmarks();
            TotalNbrofrows += itab_ptr->getNbrofrows();
            TotalNbrofcols = max(TotalNbrofcols, itab_ptr->getNbrofcols());
            itabvec.push_back(itab_ptr);

        }
        cout << TotalNbrofrows << " " <<TotalNbrofcols << endl;
        oFile.open(oheadvec.oFilename.c_str());
        cout << oheadvec.oFilename << endl;
        otab.newTable(TotalNbrofrows, TotalNbrofrows);
        cout << "blub" << endl;
        updateicol(iheadmat, itabvec);
        updateNewTable(iheadmat, oheadvec, itabvec, otab);
        otab.saveTable(oFile, "\t");
        for(i=0;i<iheadmat.size();i++){
            delete itabvec[i];
        }
        oFile.close();
    }
    cout << "end" << endl;
    //cout << itab.getNbrofrows() << " " << itab.getNbrofcols() << endl;




    return 0;
}
