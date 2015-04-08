
#include <iostream>
#include <fstream>
#include <getopt.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>
#include <ctime> 
#include <algorithm>

//#include <omp.h>



#include <itpp/itcomm.h>



#include "getopt_pp.h"

#include "gldpc.h"
#include "utils.h"

//#include "sse_mathfun.h"

using namespace std;
using namespace GetOpt;




static void usage() {
    cerr << "Usage: gldpc -n <codelength> [-d <leftdegree>] -D <rightdegree> -C <channel>  [-p <channel param>] ";
    cerr << "\t[-e <erasure>] [-r <erasure range>] [-I <iterations=1>] [-w <graphml file>] [-q] [-x <randomseed>] [-g <stopcond>] [-t <serthresh>] [-o <outputfile>] [-p time] -E erasurefile" << endl;
    cerr << "\tuse -w to export the code structure to a graphml file, no simulation" << endl;
    cerr << "\t-e <<erasure> , the default value = 1-rate-(1-rate)/5 " << endl;
    cerr << "\t-q\tquiet mode, least verbose" << endl;
    cerr << "\t-x\trandom seed, if not specified use the system time." << endl;
    cerr << "\t-g <stopcond>\tbinary search to find the capacity gap" << endl;
    //./simcodes -c st -n 10 -s 128 -d 4 -m 5
    //./simcodes -c ex -n 14 -s 256 -d 4 --md1 7 --md2 15 -I 20 -g  
    exit(EXIT_FAILURE);
}



int main(int argc, char* argv[]) {
    int n = 512;
    int d=2,D=8;
    unsigned int niter = 1;
    unsigned int bpiter=50;
    double capacity=0.5;    
    int ch=1;
    Channel *channel;
    double channelparam=0.23;
    bool quiet = false;
    string gname = "";
    unsigned long int rseed = static_cast<unsigned long int>(std::time(0));
    GetOpt_pp ops(argc, argv);

    if (ops >> OptionPresent('h', "help") || argc == 1)
        usage();
    //required args
    try {
        ops.exceptions(std::ios::failbit | std::ios::eofbit);
        ops 
                >> Option('n',"nvar", n) ;                                              
                //>> Option('d', "d1", d1);
                //>> Option('m', "md1", md1);
    } catch (const GetOptEx& e) {
        std::cerr << "Invalid options or format\n";
        usage();
    }
    //optional args
    try {
        ops.exceptions(std::ios::failbit);
        ops 
                >>  Option('d', d)
                >>  Option('D', D)
                >>  Option('p', channelparam)
                >>  Option('c', "channel", ch)                
                >>  Option('i', "bpiter", bpiter)
                >>  Option('I', "iterations", niter, niter)
                >>  Option('x', "seed", rseed, rseed);

    } catch (const GetOptEx& e) {
        std::cerr << e.what();
        std::cerr << "Invalid options or format\n";
        usage();
    }
    quiet = ops >> OptionPresent('q', "quiet");
    
    if(!quiet)
        cout<<"Random seed is "<<rseed<<endl;


     
#ifdef USE_PRERAND
     string randfile="/home1/masoud/yearly/2010.bin";
    unsigned int nr=ptrand.openrandfile(randfile.c_str(),32, 25000000);
    if(!nr){
        cerr<<"Could not open file "<<randfile<<"..."<<endl;
        exit(EXIT_FAILURE);
    }
    cerr<<"There are "<<nr<<" 32bit random numbers in "<<randfile<<endl;
#endif     
   
    setrandseed(rseed);
    
    
    
    switch (ch){
        case 2:
            channel=new AWGN(channelparam);
            break;
        case 1:
            channel=new BSC(channelparam);
            break;
        case 0:
            break;
        default:
            cerr<<"Unsupported channel"<<endl;
            exit(-1);
    }    
    if(ops >> OptionPresent("capacity")){
        ops >>  Option("capacity", capacity);
        channel->setCapacity(capacity);
    }
    capacity=channel->capacity();
    

    
    precomputedTables();

     
    /*RandomLinearCode test(6,5);
    test.generate();
    test.enumerateCodewords();
    test.print();*/
    
    //return 0;
    GLDPC code(n,D,d);
    RateDistribution rates;
    code.InstantiateGraph(rates,capacity);
    if(!quiet){
        channel->print();
        code.print();       
    }
   
    //cout.flush();
      
    vector<LLRType> llrch(code.length(),0.0);
    vector<LLRType> llrbp(code.length(),0.0);
    vector<bool> decoded(code.length(),1);
    itpp::BERC berc; 
    itpp::bvec bitsin = itpp::zeros_b(code.length());
    
    for(unsigned i=0;i<niter;i++){
        channel->transmitzero(llrch);        
        //PrintVector(cout,llrch,",");
        code.bp_decode(llrch,llrbp,bpiter); 
        PrintVector(cout,llrbp,",");
        channel->decode(llrbp,decoded);
    }
    cout<<endl;
    delete channel;
}
