#ifndef EBC_H 
#define EBC_H

#include <string>
#include <vector>
#include "graph.h"

class EBC //Expander Block Code
{
public:
    EBC(int nb1, int nb2, int sb1, int sb2);
    ~EBC(void);

    BipartiteGraph graph;

    int sb1, sb2; // "s"ize of "b"locks in the first and second dimensions
    int *md1, *md2;
    std::vector<int> K1, K2;
    std::vector<std::vector<char> > systematic;
    int codeLength;
    int nConstraints;
    int nInfSymbols;
    double designRate(void);
    int rateUpperBound(void);
    void setRegularMD(int md1, int md2);
    void setRegularMD(std::vector<int>& MD1,std::vector<int>& MD2) ;
    void setUniformRandomMD();
    bool readMDFile(std::string fname);
    void setSubcodeInf();
    void setSubcodeInf(std::vector<int> a, std::vector<int> b);
    void testSystematicProcedure(int nperm);
    void makeCheckMatrix();
    int CheckMatrixRank(void *f);
    void printMD();
    void printParams();
    void printSystematic();
    unsigned int nCheckOnCheck();
};

class ErasurePattern {
public:
    ErasurePattern(const EBC& code);
    ErasurePattern(const EBC& code, double erasureProb);
    ErasurePattern(const EBC& code, std::string fname);

    BipartiteGraph pattern;
    //static base_srng_type RandomGen;
    //const EBC code;
    unsigned int instance(const EBC& code, double erasureProb);
    void decode(const EBC& code);
};




#endif