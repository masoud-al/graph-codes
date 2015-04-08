/* 
 * File:   GLDPC.cpp
 * Author: masoud
 * 
 * Created on February 13, 2012, 7:42 PM
 */
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <queue>
#include <algorithm>
#include <set>
#include <cmath>

#include "utils.h"

#ifdef USE_ITPP
#include <itpp/itcomm.h>
#endif



#include "channel.h"
#include "utils.h"

using namespace std;


//-------------------------------- Channels ---------------------------
Channel::Channel(double p) {
    this->param=p;
}
BSC::BSC(double p):Channel(p){
#ifdef USE_ITPP
    ch=new itpp::BSC(p);
#endif
}

BSC::~BSC(){
#ifdef USE_ITPP
    if(ch) delete (itpp::BSC*)ch;
#endif

}

double BSC::capacity(double p){
    return 1.0+p*log2(p)+(1-p)*log2(1-p);
}



double BSC::setCapacity(double c){
    double p1=0.0,p,p2=0.5,cap;
    do{
        p=(p1+p2)/2.0;
        cap=1.0+p*log2(p)+(1-p)*log2(1-p);
        if(cap<c)
            p2=p;
        else
            p1=p;
    }
    while(abs(cap-c)>1e-10);        
    param=p;    
    return param;
}

unsigned int BSC::transmitzero(std::vector<LLRType>& llr){
    unsigned int N=llr.size();
    LLRType p=(LLRType) param;
    LLRType v0=mylog(p/(1.0-p));
    for (unsigned int i=0;i<N;i++){
        llr[i]=UniformRandom<param?-v0:v0;
    }
	return 0;
}

unsigned int BSC::transmitzero(LLRType* llr, unsigned int N){
    LLRType p=(LLRType) param;
    LLRType v0=mylog((1.0-p)/p);
	unsigned int nerr=0;
    for (unsigned int i=0;i<N;i++){
		if(UniformRandom<param){ //flip
			llr[i]=-v0;
			nerr++;
		}else
			llr[i]=v0;
    }
	return nerr;
}

/*
void BSC::decode(const std::vector<LLRType>& llr,std::vector<bool>){
    
}
*/

void BSC::print(){
    cout<<"BSC("<<param<<"), capacity="<<capacity()<<endl;
}

AWGN::AWGN(double p):Channel(p){
#ifdef USE_ITPP
    ch=new itpp::AWGN_Channel(p);
#endif
	variance=SNRtoVariance(param);
}

AWGN::~AWGN(){
#ifdef USE_ITPP
    if(ch) delete (itpp::AWGN_Channel*)ch;
#endif 

}

double AWGN::capacity(double p){
    return std::log(1+p)/std::log(2.0);
}

double AWGN::setCapacity(double c){
	double p1=-1.0,p,p2=10.0,cap;
    do{
        p=(p1+p2)/2.0;
        cap=capacity(p);
        if(cap<c)
            p2=p;
        else
            p1=p;
    }
    while(abs(cap-c)>1e-10);        
    param=p;    
    return param;
	variance=SNRtoVariance(param);
    return param;
}

void AWGN::print(){
    cout<<"AWGN(SNR="<<param<<",sigma="<< variance << "), capacity="<<capacity()<<endl;
}

unsigned int AWGN::transmitzero(std::vector<LLRType>& llr){
    unsigned int N=llr.size();
	return 0;
}

unsigned int AWGN::transmitzero(LLRType* llrs, unsigned int N){
	unsigned int nerr=0;	
	RandomLib::NormalDistribution<> n;
    for (unsigned int i=0;i<N;i++){
		//llrs[i]=2.0*(n(RandomLib::Random::Global)*variance+1)/variance;
		llrs[i]=2.0*n(RandomLib::Random::Global)+2.0/variance;
		if(llrs[i]<0)
			nerr++;
    }
	return nerr;
}
     


