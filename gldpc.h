/* 
 * File:   GLDPC.h
 * Author: masoud
 *
 * Created on February 13, 2012, 7:42 PM
 */

#ifndef GLDPC_H
#define	GLDPC_H

#include <vector>
#include <m4ri/m4ri.h>
#include "graph.h"

#include "channel.h"



//this class should also work for none-linear codes
class ComponentCode{
public: 
    ComponentCode(unsigned int,unsigned int);
    ~ComponentCode();
    unsigned int k,n;
    double rate;
    unsigned int size;//size of the code 
    void* words;
    LLRType *LLR;
    //virtual bool isImprovable();
    //virtual  improve();
    inline bool istrivial(){return n==k;}
    inline unsigned int length(){return n;}
    //virtual void BPUpdate(std::vector<LLRType>& LLR)=0;
    virtual void BPUpdate(LLRType *);
    //virtual void* checkMat();
};

class RandomLinearCode : public ComponentCode{
private:
    
    mzd_t *G,*H;
public:    
    
    RandomLinearCode(unsigned int n,unsigned int k);
    ~RandomLinearCode();
    void generate();
    void enumerateCodewords();
    //void BPUpdate(LLRType*);    
    void* checkMat(){return H;}
    void print();
};


class RateDistribution{
public:
    RateDistribution();
    std::vector<unsigned int> discreteharmonic(double maxvalue,double scale,int N);
};


class GLDPC {
public:
    unsigned int d,D,nVars,nFactors;
    BipartiteGraph *graph;
    std::vector<unsigned int> coDims;
    std::vector<unsigned int> coDimCount;
    std::vector< std::vector<unsigned long> > VarIndices;
    unsigned int length(){ return nVars;}
    double designrate();
    std::vector<ComponentCode*> factors;
    LLRType* LLRs;
    GLDPC(int N,int D=4, int d=2);
    void InstantiateGraph(RateDistribution rd, double capacity);
    GLDPC(const GLDPC& orig);
    void SumProductIteration();
    void channelLLR(const std::vector<LLRType>&);
    void bp_decode(const std::vector<LLRType>& llrin,std::vector<LLRType>& llrout, unsigned int niter);
    void print();
    virtual ~GLDPC();
private:
};

#endif	/* GLDPC_H */
