/* 
 * File:   utils.h
 * Author: masoud
 *
 * Created on February 17, 2012, 9:45 AM
 */

#ifndef UTILS_H
#define	UTILS_H


#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <stdint.h>

#include <RandomLib/NormalDistribution.hpp>

//uint64_t for check degree up to 64
//for larger degree one should use a bitarray type such as
// std::bitset  or boost::dynamic_bitset
typedef uint32_t Bitset;
#define MaxCheckDegree 32

#define GetBit(s,i) 		((s) & ( 1 << (i) )) >> (i)
#define SetBit(s,i) 		(s) |= 1 << (i)
#define ClearBit(s,i) 		(s) &= ~(1 << (i))
#define ToggleBit(s,i)		(s) ^= 1 << (i)
#define CheckBit(s,i)		(s) & (1 << (i))
#define EmptySet(s)			(s)=0;
#define IsEmptySet(s)		((s)==0)
#define Union(s1,s2)		((s1) | (s2))
#define Intersection(s1,s2) ((s1) & (s2))

#ifdef _MSC_VER /* visual c++ */
#define log2(x)				log(x)/log(2.0) //log(2)=0.69314718056 
#endif


#define POW2(n) (1<<(n))

/* Computing the Hamming weight efficiently
 * GCC 3.4 supports a "population count" builtin, which may be implemented by a
 * a single instruction */

#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)

#define Weight(i)		__builtin_popcount(i)

#else

//The Swar algorithm
static inline unsigned int Weight(Bitset i){    
      i = i - ((i >> 1) & 0x55555555);
      i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
      return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;   
}

#endif


#define IsOdious(s) (Weight(s)%2)



#ifdef HAVE_M4RI
#include <m4ri/m4ri.h>
mzd_t *AllBinVectors(unsigned int);
#endif





//#include "sse_mathfun.h"

/*
#include <boost/random/mersenne_twister.hpp>  // mt19937
//#include <boost/random/linear_congruential.hpp>  //minstd_rand

// one of the :  boost::mt19937 or boost::ecuyer1988 , boost::minstd_rand
typedef boost::mt19937 base_srng_type;
 */

/*
#include <boost/random/variate_generator.hpp>
//#include <boost/random/uniform_01.hpp>
//#include  <boost/random/uniform_real.hpp>

#include <boost/random/bernoulli_distribution.hpp>
#include <boost/random/uniform_int.hpp>


typedef boost::variate_generator<base_srng_type&, boost::uniform_int<> > GenTypeUniformInt;
typedef boost::variate_generator<base_srng_type&, boost::bernoulli_distribution<> > GenTypeBernoli;
//typedef boost::variate_generator<base_srng_type&, boost::uniform_real  > GenTypeReal;
//typedef boost::variate_generator<base_srng_type&, boost::uniform_01  > GenTypeRealUnit;
 */

/*
#include <boost/generator_iterator.hpp>
 GenTypeBernoli flip_gen(RandomGen, boost::bernoulli_distribution(0.5));
 boost::generator_iterator<GenTypeBernoli> flip(&flip_gen);
  then  *flip++ is like flip()
 */

/* boost::uniform_int<> vdist(0, nVar-1);
    boost::variate_generator<base_srng_type&, boost::uniform_int<> > rvar(RandomGen, vdist);
    boost::bernoulli_distribution bern(0.5);
    boost::variate_generator<base_srng_type&, boost::uniform_int<> > flip(RandomGen, bern);*/

/* check for all macros
 * RandInteger(n)  : integer in [0,n)
 * UnbiasedBit     : unbiased coin
 * UniformRandom   : uniform double in [0,1)
 * 
 * 
 * 
 * 
 * 
 * 
 */

// RandInteger(n) returns random int in [0,n)

// CRT

#ifdef USE_CRTRAND
#define RandInteger(n) int((n) * double(std::rand())/(RAND_MAX + 1.0)) 
//#define RandInteger(n) (rand() % (n))
#define UniformRandom double(std::rand())/(RAND_MAX + 1.0)  
#define UnbiasedBit (rand()%2)

#elif USE_RANDOMLIB
#include <RandomLib/Random.hpp>
//#include <RandomLib/RandomSelect.hpp>
#define RandInteger(n) RandomLib::Random::Global.Integer(n)
#define UnbiasedBit  RandomLib::Random::Global.Boolean()        
#define Bernouli(p)  RandomLib::Random::Global.Prob(p)
#define UniformRandom RandomLib::Random::Global.Fixed<double>()

#elif defined USE_POSIX  

#define UniformRandom drand48()

#endif


inline void setrandseed(unsigned int rseed){
	 
#ifdef USE_CRTRAND
	   if(!rseed)
		   rseed=static_cast<unsigned int>(std::time(0));
	   std::srand(rseed);
#elif USE_RANDOMLIB      
	   if(rseed)
        RandomLib::Random::Global.Reseed(rseed);
	   else 
        RandomLib::Random::Global.Reseed();
#endif
}

//base_srng_type BipartiteGraph::RandomGen;

//extern base_srng_type RandomGen;


#ifdef USE_PRERAND
#include "PreRandom.h"
extern PreRandom ptrand;       
#endif


/*class RandomGenerator{
    
};*/






#define MAXIMUM_CODE_DIMENSION 16

void precomputedTables();



inline void permute(int *a, unsigned int n) {
    for (unsigned int i = 1; i <= n; i++) {
        unsigned int j =  RandInteger(i);
        int temp = a[i - 1];
        a[i - 1] = a[j];
        a[j] = temp;
    }
}



template <class T>
inline void permute(std::vector<T>& a) {
    for (unsigned int i = 1; i <= a.size(); i++) {
        unsigned int j = RandInteger(i);
        T temp = a[i - 1];
        a[i - 1] = a[j];
        a[j] = temp;
    }
}

/*  k distinct random integer in [0,n)   */
inline void Durstenfeld_shuffle(int *a, unsigned int n, unsigned int k) {
    assert(k>=1 && k<=n-1);
    for (unsigned int i = n-1; i >= n-k; i--) {
        unsigned int j =  RandInteger(i+1);
        int temp = a[i];
        a[i] = a[j];
        a[j] = temp;
    }
}

template <class T>
inline void Durstenfeld_shuffle(std::vector<T>& a, unsigned int k) {
    assert(k>=1 && k<=a.size()-1);
    for (unsigned int i = a.size()-1; i >= a.size()-k; i--) {
        unsigned int j = RandInteger(i+1);
        T temp = a[i];
        a[i] = a[j];
        a[j] = temp;
    }
}


bool readfile(std::vector< std::vector<int> >& vals,const char* fname);




template<typename T>
void PrintVector(std::ostream& ostr, const std::vector<T>& t, const std::string& delimiter){
        std::copy(t.begin(), t.end(), std::ostream_iterator<T>(ostr, delimiter.c_str()));
}

#endif	/* UTILS_H */

