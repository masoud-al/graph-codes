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


#include <itpp/itcomm.h>

//#include <m4rie/m4rie.h>
//#include <m4rie/finite_field_givaro.h>



#include "gldpc.h"
#include "utils.h"

using namespace std;



//-------------------------------- RandomLinearCode ---------------------------
ComponentCode::ComponentCode(unsigned int n,unsigned int k){
    this->n=n;
    this->k=k;
    words=NULL;
    LLR=new LLRType[n];
}

ComponentCode::~ComponentCode(){
    delete[] LLR;
}

void ComponentCode::BPUpdate(LLRType *LLRin){
     assert(words);
     mzd_t *codewords=(mzd_t*)  words;
     unsigned int codesize=codewords->nrows;
     assert((unsigned int)codewords->ncols==n);
     for(unsigned int j=0;j<n;j++){
         for(unsigned int c=0;c<codesize;c++){
             for(unsigned int i=0;i<n;i++){
                 ;
             }
         }
     }
}

RandomLinearCode::RandomLinearCode(unsigned int n,unsigned int k):ComponentCode(n,k){
    G=H=NULL;
}

RandomLinearCode::~RandomLinearCode(){
    if(G)
        mzd_free(G);
    if(H)
        mzd_free(H);
    if(words) 
        mzd_free((mzd_t*)words);
}
void RandomLinearCode::generate(){
     if(G) mzd_free(G);
     G=mzd_init(k,n);
     if(k==n-1){ //parity code
         for(unsigned int j=0;j<k;j++){
             mzd_write_bit(G,j,k,1);//last col
             mzd_write_bit(G,j,j,1);//identiy
         }
         return;
     }
         
     unsigned int rank=0,row=0;
     while (rank<k){
        for(unsigned int j=0;j<n;j++)
          mzd_write_bit(G,row,j,UnbiasedBit);
        rank=mzd_echelonize_m4ri(G,1,0);
        row+=rank>row;
     }
}

void RandomLinearCode::enumerateCodewords(){

    assert(k<=MAXIMUM_CODE_DIMENSION);
 /*   mzd_t *codewords=mzd_init(POW2(k),n);
    words=codewords;
    //choose a submatrix
    //mzd_t *bin2k=mzd_init_window(AllBinaryVectors,0,0,POW2(k),k);
    mzd_t *bin2k=AllBinVectors(k);
    //mzd_print(bin2k);
    mzd_mul_m4rm(codewords, bin2k, G, 0);
    //mzd_print(codewords);
    mzd_free_window(bin2k); */
}

void RandomLinearCode::print(){
    mzd_print(G);
    //mzd_print((mzd_t*)words);
}

/* to implement more efficiently than bruteforce
void RandomLinearCode::BPUpdate(LLRType* LLRs){

}
  */
//-------------------------------- RateDistribution ---------------------------
RateDistribution::RateDistribution(){
    ;
}

/* returns a vector of length ceil(maxvalue) containing the */
std::vector<unsigned int> RateDistribution::discreteharmonic(double maxvalue,double scale,int N){
    vector<unsigned int> counts;
    counts.push_back(0);
    int nonzero=0;
    int k,j=1;
    while(j<=ceil(maxvalue) && j<4){
        k=round(scale*N/j);
        assert(k>0);
        counts.push_back(k);
        nonzero+=k;
        j++;
    }
    counts[0]=N-nonzero;
    return counts;
}

//-------------------------------- GLDPC --------------------------------------

GLDPC::GLDPC(int N,int D,int d) {
    this->nVars=N;
    this->D=D;
    this->d=d; 
    this->nFactors=ceil(nVars*d/(double)D);
    graph=NULL;
    LLRs=NULL;
}

GLDPC::GLDPC(const GLDPC& orig) {
    
}

GLDPC::~GLDPC() {
    if(graph)
        delete graph;
    if(LLRs)
        delete[] LLRs;
    for (unsigned int j = 0; j < factors.size(); j++)
        if(factors[j])
            delete factors[j];
}

void GLDPC::InstantiateGraph(RateDistribution rd, double capacity){
    coDimCount=rd.discreteharmonic((1-capacity)*D,1.0/d,nFactors);
    coDims.clear();
    for(unsigned int j=0;j<coDimCount.size();j++)
        for (unsigned int i=0;i<coDimCount[j];i++)
            coDims.push_back(j);
    assert(coDims.size()==nFactors);
    //permute(coDims);    
    graph=new BipartiteGraph(nVars,nFactors);
    if(!graph->makeRandomSimpleRegular(d,D)){
        cerr<<"Couldn't make the bipartite graph"<<endl;
        return;
    }
    graph->printAdjacencyLists();
    graph->printAdjacencyMatrix();
    //print();
    //adding random linear codes as factors
    for (unsigned int j = 0; j < nFactors; j++) {
        unsigned int length=graph->deg2[j];
        if (!coDims[j]){ //trivial code
            factors.push_back(new ComponentCode(length,length-coDims[j]));
            continue;
        }
                
        //cout<<"deg[j]="<<length<<",codim="<<coDims[j]<<endl;
        assert(coDims[j]<length);
        RandomLinearCode* rlc=new RandomLinearCode(length,length-coDims[j]);
        rlc->generate();
        rlc->enumerateCodewords();
        factors.push_back(rlc);
    }
    
    LLRs=new LLRType[graph->nEdges()];
    /*stroring LLRs acording to Factor perspective
     *  Factor 0 -> LLR[0...graph->deg2[0]-1]
     *  Factor 1 -> LLR[graph->deg2[1]...]
     * 
     */
   
    vector<unsigned long> tmp;tmp.resize(d);
    VarIndices.resize(graph->n1);
    for (int i = 0; i < graph->n1; i++) 
        VarIndices.push_back(tmp);
    unsigned long e=0;
    for (int j = 0; j < graph->n2; j++) 
        for (Cell* iterator = &(graph->v2[j]); iterator->down != &(graph->v2[j]); iterator = iterator->down)
            VarIndices[iterator->down->row].push_back(e++);    
     
    /*for (unsigned int i = 0; i < nVars; i++) {   
           PrintVector(cout,VarIndices[i]," ");
           cout<<endl;
    }*/
}

void GLDPC::print(){
    printf("--------------------------------------------------------------\n");
    printf("nVars=%d, nFactors=%d, d=%d, D=%d\n",nVars,nFactors,d,D);
    
    printf("codims counts:\n");
    for(unsigned int j=0;j<coDimCount.size();j++) printf("%d\t",j);
    printf("\n");
    for(unsigned int j=0;j<coDimCount.size();j++) printf("%d\t",coDimCount[j]);
    printf("\n");
    
    //graph->printAdjacencyLists();
    /*
    printf("coDims:\n");
    for(unsigned int j=0;j<coDims.size();j++) printf("%d\t",coDims[j]);
    printf("\n");*/
    printf("--------------------------------------------------------------\n");
}

void GLDPC::channelLLR(const std::vector<LLRType>& cllr){
    assert(cllr.size()==nVars);
    for (unsigned int i = 0; i < nVars; i++)    
       for (unsigned int k= 0; k < VarIndices[i].size(); k++)
           LLRs[VarIndices[i][k]]=cllr[i];
}

void GLDPC::bp_decode(const std::vector<LLRType>& llrin,std::vector<LLRType>& llrout, unsigned int niter)
{
   assert(llrin.size()==nVars);
   assert(llrout.size()==nVars);
   assert(factors.size()==nFactors);
   for (unsigned int i = 0; i < nVars; i++)    
       for (unsigned int k= 0; k < VarIndices[i].size(); k++)
           LLRs[VarIndices[i][k]]=llrin[i];
   
   for (unsigned int it = 0; it < niter; it++)
       SumProductIteration();
    
   for (unsigned int i = 0; i < nVars; i++)   
       llrout[i]=LLRs[VarIndices[i][0]];
}

void GLDPC::SumProductIteration(){    
   //Factors 
   unsigned long e=0; 
   for (unsigned int j = 0; j < factors.size();j++){
       if(!factors[j]->istrivial())
         factors[j]->BPUpdate(LLRs+e);
       e+=factors[j]->length();  
   }
   //add them at variable nodes 
   for (unsigned int i = 0; i < nVars; i++){
       LLRType sum=0.0;
       for (unsigned int k= 0; k < VarIndices[i].size(); k++) 
           sum+=LLRs[VarIndices[i][k]];
       for (unsigned int k=0; k < VarIndices[i].size(); k++) 
           LLRs[VarIndices[i][k]]=sum-LLRs[VarIndices[i][k]];
   }
     
}

