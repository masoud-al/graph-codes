/* 
 * File:   PreRandom.h
 * Author: masoud
 *
 * Created on January 20, 2012, 5:37 AM
 */

#ifndef PRERANDOM_H
#define	PRERANDOM_H


#include <iostream>
#include <fstream>

#include <limits>
#include <cstdint>

typedef uint32_t RandType;

class PreRandom {
public:
    PreRandom();
    PreRandom(short bits, unsigned int inbuf);
    unsigned int openrandfile(const char *fname, int bits, int inbuf);
    //RandType next();

    inline RandType next() {
        if (curpos == inbuffercount)
            readnextchunk();
        return buffer[curpos++];
    }
    //double unituniform();

    inline double unituniform() {
        RandType r = next();
        return r / 4294967295.0;
    }
    virtual ~PreRandom();
private:
    std::ifstream rndfile;
    RandType* buffer; // should be change to template 
    unsigned int curpos;
    unsigned int buffersize;
    unsigned int inbuffercount;
    unsigned int filepos;
    //bool readnextchunk();

    inline bool readnextchunk() {
        std::cerr<<"reading another chunk..."<<std::endl;
        curpos = 0;
        rndfile.read((char*) buffer, buffersize);

        return true;
    }

};


#endif	/* PRERANDOM_H */

