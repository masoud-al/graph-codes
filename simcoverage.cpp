/* 
 * File:   simcoverage.cpp
 * Author: masoud
 *
 * Created on July 6, 2012, 5:31 PM
 */

#include <cstdlib>
#include <iostream>
#include <ctime> 
#include "getopt_pp.h"
#include "utils.h"

using namespace std;
using namespace GetOpt;
/*
 * 
 */


unsigned int CouponCollector( unsigned int N,unsigned int d){
    int *a=new int[N]; for(unsigned int i=0;i<N;i++) a[i]=i;
    bool *V=new bool[N]; for(unsigned int i=0;i<N;i++) V[i]=false;
    unsigned int nv=0,k=0;    
    while (nv<N){
        Durstenfeld_shuffle(a,N,d);
        //a[N-1]=RandInteger(N);
        for(unsigned int j=1;j<=d;j++){   
            unsigned int t=a[N-j];//RandInteger(N);
            if (!V[t]){
                V[t]=true;
                nv++;
            }        
        }
        k++;
        //for(unsigned int i=0;i<N;i++) a[i]=i;
    }
    delete[] a;delete[] V;
    return k;
}

unsigned int wallrandomwalk( unsigned int N,unsigned int d){
    int *a=new int[N]; for(unsigned int i=0;i<N;i++) a[i]=i;
    int *C=new int[N]; for(unsigned int i=0;i<N;i++) C[i]=1;
    unsigned int k=0;
    while (true){
        unsigned int b1=RandInteger(N);
        unsigned int b2=RandInteger(N-1);
        if (b1==b2)
            b2=N-1;
        
        if(a[b1]!=a[b2]){
            C[a[b1]]--;
            C[a[b2]]++;
            a[b1]=a[b2];
        }

        if(C[a[b2]]==N)
            break;
        k++;        
    }
    delete[] a;delete[] C;
    return k;
}

unsigned int PonderThisJuly12( unsigned int N,unsigned int M){
    int *a=new int[N]; for(unsigned int i=0;i<N;i++) a[i]=i;
    int *C=new int[N]; for(unsigned int i=0;i<N;i++) C[i]=1;
    bool *V=new bool[M]; for(unsigned int i=0;i<M;i++) V[i]=false;
    unsigned int mv=0,k=0;    
    while (true){
        unsigned int b1=RandInteger(N);
        unsigned int b2=RandInteger(N-1);
        if (b1==b2)
            b2=N-1;
        
        if(a[b1]!=a[b2]){
            C[a[b1]]--;
            C[a[b2]]++;
            a[b1]=a[b2];
        }

        if(C[a[b2]]==N)
            break;
       
        unsigned int t=RandInteger(M);
        if (!V[t]){
            V[t]=true;
            mv++;
        }            
        if(mv==M)
            break;
        k++;        
    }
    delete[] a;delete[] V;delete[] C;
    return mv==M?1:0;
}

int main(int argc, char** argv) {
    unsigned long int rseed = static_cast<unsigned long int>(std::time(0));
    unsigned int N=10,M=20,d=2,it=1000;
    
    GetOpt_pp ops(argc, argv);
    try {
        ops.exceptions(std::ios::failbit);
        ops 
                >>  Option('n', N)
                >>  Option('m', M)
                >>  Option('d', d)
                >>  Option('i', it)
                >>  Option('x', "seed", rseed, rseed);

    } catch (const GetOptEx& e) {
        std::cerr << e.what();
        std::cerr << "Invalid options or format\n";
        //usage();
    }
    setrandseed(rseed);
    
    //vector <unsigned int> a;
    /*int *a=new int[N]; for(unsigned int i=0;i<N;i++) a[i]=i+1;
    for(unsigned int i=0;i<it;i++){
        //Durstenfeld_shuffle(a,d);
        Durstenfeld_shuffle(a,N,d);
        for(unsigned int j=1;j<=d;j++)
            cout<<a[N-j]<<" ";
        cout<<endl;
    }
    delete[] a;
     */ 
    unsigned int s=0;
    for(unsigned int i=0;i<it;i++){
        //cout<<CouponCollector(N,d)<<endl;
        //cout<<PonderThisJuly12(N,M)<<endl;
        cout<<wallrandomwalk(N,d)<<endl;
        //s+=PonderThisJuly12(N,M);
    }
    return 0;
    cout<<"N="<<N<<", M="<<M<<endl;
    cout<<(double)s/it<<endl;
    
    return 0;
}

