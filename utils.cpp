/* 
 * File:   utils.cpp
 * Author: masoud
 * 
 * Created on February 17, 2012, 9:45 AM
 */


#include "utils.h"


#ifdef USE_RANDOMLIB
//#include "PreRandom.h"    
#endif

#ifdef USE_PRERAND
PreRandom ptrand;       
#endif

using namespace std;

#ifdef HAVE_M4RI
mzd_t *AllBinaryVectors = NULL;

void precomputedTables() {
    AllBinaryVectors=mzd_init(POW2(MAXIMUM_CODE_DIMENSION), MAXIMUM_CODE_DIMENSION);
    for(word t=0;t<POW2(MAXIMUM_CODE_DIMENSION);t++)
        *AllBinaryVectors->rows[t]=t;
    //mzd_print(AllBinaryVectors);
}

mzd_t * AllBinVectors(unsigned int length){
    assert(length<=MAXIMUM_CODE_DIMENSION);
    return mzd_init_window(AllBinaryVectors,0,0,POW2(length),length);
}
#endif







bool readfile(vector< vector<int> >& vals,const char* fname){
  ifstream infile(fname,ifstream::in);
  if(!infile){
    cerr<<"couldn't open file "<<fname<<endl;
    return false;
  }
 string aline;
 while(getline(infile,aline))
 {
        stringstream  lineStream(aline );
	//cout<<"->"<<aline<<endl;
        string   x;
	vector<int> dims;
        do
        {
	   int z=-1;
	   lineStream>>z;
	   //cout<<z<<" ";
	   if(z!=-1)
	    dims.push_back(z);
        }while(getline(lineStream,x,'\t')  && infile.good()   );
        //cout<<endl;
        vals.push_back(dims);
  }
    
  fprintf(stderr,"read %d lines from file %s, each having size %d\n",
          (unsigned int)vals.size(),fname,(unsigned int)vals[0].size());

  infile.close();
  return true;
} 