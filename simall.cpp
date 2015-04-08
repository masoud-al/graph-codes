#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include "ebc.h"

#include <m4ri/m4ri.h>
#include <m4rie/m4rie.h>
#include <m4rie/finite_field_givaro.h>

using namespace std;


vector<int>&  parsevector(const string& line){
    vector<int>& dims;
    stringstream  lineStream(line);
    string   x;
    do
    {
       int z=-1;
       lineStream>>z;
       if(z!=-1)
          dims.push_back(z);
    }while(getline(lineStream,x,'\t'));
    return dims;
}


vector<int>& readline(const char* fname,int ln){
  ifstream infile(fname,ifstream::in);
  if(!infile){
    cerr<<"couldn't open file "<<fname<<endl;
    return false;
  }
 string aline;
 
 while(getline(infile,aline) && ln--)
     ;
 infile.close();
 return parsevector(aline);
      
 // fprintf(stderr,"read %d lines from file %s, each having size %d\n",(unsigned int)vals.size(),fname,(unsigned int)vals[0].size());
}

int main(int argc, char** argv){
  if(argc<3){
    cerr<<"usage: ./allrate rowfile row# colfile col#"<<endl;
    return 1;
  }
  
  vector< vector<int> > rowdims;
  vector< vector<int> > coldims;
  if(!readfile(rowdims,argv[1])){
    return 1;
  }
  
  if(!readfile(coldims,argv[2])){
    return 1;
  }
  
  int k=0,step=ceil(rowdims.size()*coldims.size()/200.0);
  EBC code(1, 1, rowdims[0].size(), coldims[0].size());
  code.graph.makeStaircase(1);
  
  /*for(unsigned int i=0;i<rowdims.size();i++){
    for(unsigned int j=0;j<rowdims[0].size();j++)
      printf("%d\t",rowdims[i][j]);
    printf("\n");
  }
  return 0;*/
  
   Givaro::GFqDom<int> GF = Givaro::GFqDom<int>(2,8);
   M4RIE::FiniteField *F = (M4RIE::FiniteField*)&GF;
   gf2e *ff=gf2e_init_givgfq(F);
  
  for(unsigned int i=0;i<rowdims.size();i++){
    for(unsigned int j=0;j<=i;j++){
      code.setSubcodeInf(rowdims[i],coldims[j]);
      //code.printMD();
      //continue;cout<<endl;
      int dim1=code.rateUpperBound(); 
      int dim2=code.CheckMatrixRank(ff);
      printf("%d\t%d\t%d\t%d\n",i,j,dim1,dim2);
      if(! (++k%step)) cerr<<"progress: %"<<k/step<<endl;
    }
  }
  cerr<<"done."<<endl;
  return 0;
}