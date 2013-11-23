#include <iostream>
#include "Table.h"
#include <fstream>
#include <string>
#include "extra.h"
#include "configread.h"

using namespace std;




int updateicol(vector<headpairs_t> &headvec, Table &itab)
{
    unsigned i, icol;
    string str;
    for(i=0;i<headvec.size();i++){
        for(icol=0;icol<itab.getNbrofcols();icol++){
            str = itab.Tableread(0, icol);
            if((headvec[i].iname == str) || (headvec[i].namecontains && std::string::npos!=str.find(headvec[i].iname)) || (headvec[i].strcontainsname && (std::string::npos!=headvec[i].iname.find(str)))){
                headvec[i].icol = icol;
            }
        }
    }
    return 0;
}

int updateNewTable(vector<headpairs_t> &headvec, Table &itab, Table &otab)
{
    unsigned i, row;
    for(i=0;i<headvec.size();i++){
        otab.Tablewrite(0, headvec[i].ocol, headvec[i].takeDatabasefieldname?itab.Tableread(0,headvec[i].icol):headvec[i].oname);
        for(row=1;row<itab.getNbrofrows();row++){
            otab.Tablewrite(row, headvec[i].ocol, itab.Tableread(row, headvec[i].icol));
        }
    }
    return 0;
}

int main()
{
    Table itab;
    Table otab;
    ifstream iFile;
    ofstream oFile;
    vector<headpairs_t> head2headvec;
    headpairs_t head2head;
    configread conf;
    configstate_et state;
    unsigned row;
    string iFilename, oFilename;

    conf.loadConfig("digikey2kicadschpatcher.conf");
    state = NONE;
    while(EOFile!=state){
        state = conf.getConfig(row, head2headvec, iFilename, oFilename);
        cout << state << " " << row << endl;
        head2headvec.clear();
        iFile.open(iFilename);
        oFile.open(oFilename);
        itab.loadTable(iFile);
        otab.newTable(itab.getNbrofrows(), head2headvec.size());
        updateicol(head2headvec, itab);
        updateNewTable(head2headvec, itab, otab);
        otab.saveTable(oFile, "\t");
    }
    cout << itab.getNbrofrows() << " " << itab.getNbrofcols() << endl;


    iFile.close();
    oFile.close();

    return 0;
}
