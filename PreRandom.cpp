/* 
 * File:   PreRandom.cpp
 * Author: masoud
 * 
 * Created on January 20, 2012, 5:37 AM
 */

#include "PreRandom.h"


using namespace std;

PreRandom::PreRandom(){
    
}

//bits : precision bits, count
PreRandom::PreRandom (short bits, unsigned int inbuf) {
    inbuffercount=inbuf;
    buffersize=sizeof(RandType)*inbuffercount;
    curpos=0;
    buffer=NULL;
}


PreRandom::~PreRandom() {
    if(rndfile.is_open())
        rndfile.close();
    if(buffer)
        delete[] buffer;
}



unsigned int PreRandom::openrandfile(const char* fname,int bits,  int inbuf){
  rndfile.open(fname, ios::in|ios::binary);  
  if(! rndfile.good())
      return 0;
  
  // get length of file:
  rndfile.seekg (0, ios::end);
  unsigned int filesize = rndfile.tellg();
  rndfile.seekg (0, ios::beg);
  inbuffercount=inbuf;
  buffersize=sizeof(RandType)*inbuffercount;
  curpos=0;
  buffer=new RandType[inbuffercount];
  
  return filesize/sizeof(RandType);
}

/*
inline RandType PreRandom::next(){
    if(curpos==inbuffercount)
        readnextchunk();
    return buffer[curpos++];   
}

inline double PreRandom::unituniform(){
    RandType r=next();
    return r/10000.0;
}

inline bool PreRandom::readnextchunk(){
    curpos=0;
    rndfile.read((char*)buffer,buffersize);
    
    return true;
}

*/