
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

#include <m4ri/m4ri.h>
#include <m4rie/m4rie.h>
#include <m4rie/finite_field_givaro.h>


 


#include "getopt_pp.h"
#include "ebc.h"
#include "PreRandom.h"
#include "utils.h"

using namespace std;
using namespace GetOpt;


long int rseed;
//base_srng_type RandomGen;



static void usage() {
    cerr << "Usage: simcodes -c <st|ex> --n1 <#blockrows> [--n2 <#blockcols> ] --sb1 <#rowsyms> [ --sb2 #colsyms> ] --d1 <deg1> [--d2 <deg2>] [--md1 <md1>] [--md2 <md2>] [--md mdfile]";
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


double gapbinsearch(EBC& code,double rate, double stopcond, double reliable, int niter,ofstream& log) {
    double e, e1 = 0, e2 = 1 - rate;
    int k = 0;
    double ber = 0, ser = 0;
  
    //bool resf=of?true:false;
    char fmt[1000];    
    while (abs(e1 - e2) > stopcond) {
        k++;
        e = (e1 + e2) / 2;
        int s = 0;
        int remsyms, nfailed = 0;
        //#pragma omp parallel for
        setrandseed(rseed);

        ErasurePattern ep(code);
        for (int i = 0; i < niter; i++) {
            ep.instance(code,e);
            ep.decode(code);
            remsyms = ep.pattern.nEdges();
            s += remsyms;
            nfailed += remsyms > 0;
        }
        ber = nfailed / (double) niter;
        ser = s / (double) (code.codeLength * niter);
        printf("%02d-rate=%f, e=%0.15f , gap=%0.15f , BER=%0.15e , SER=%0.15e , step=%0.15e\n", k, rate, e, 1 - rate - e, ber, ser, abs(e1 - e2));
        if (log.is_open()){
            sprintf(fmt,"%d\t%d\t%0.15e\t%0.15e\t%0.15e\t%d\n", code.codeLength, code.nInfSymbols,e, ber, ser,niter);
            log<<fmt;
        }
                             
        if (ber < reliable)
            e1 = e;
        else
            e2 = e;
    }
    //if (resf)
       
    return e;

}


double sampleerrorcurve(EBC& code,double erasure,int niter,ofstream& log){
    setrandseed(rseed);
    //double start=0.0, end=0.0;
    int s = 0;
    int remsyms, nfailed = 0;
    //start = omp_get_wtime();
    //#pragma omp parallel for
    for (int i = 0; i < niter; i++) {
        ErasurePattern ep(code, erasure);
        //ErasurePattern ep(code, "er.txt");
	/*if(!quiet){
	  ep.pattern.printAdjacencyMatrix();
	  ep.pattern.printDegrees();
	}*/
        ep.decode(code);
        remsyms = ep.pattern.nEdges();
        s += remsyms;
        nfailed += remsyms > 0;
    }
    //end = omp_get_wtime();
    double ber = nfailed / (double) niter;//block error 
    double ser = s / (double) (code.codeLength * niter);//symbol error
    char msg[1000];   
    sprintf(msg,"%d\t%d\t%0.15e\t%0.15e\t%0.15e\t%d\n", code.codeLength, code.nInfSymbols,erasure, ber, ser,niter);
    cout<<msg;
    if(log.is_open())
        log<<msg;
    return ber;
}

int main(int argc, char* argv[]) {
    int n1 = 1, n2 = n1;
    int sb1 = 8, sb2 = sb1;
    int d1 = 1, d2 = d1;
    int md1 = 5, md2 = md1;
    int niter = 1;
    double rate;
    string codetype="ex";
    double erasure=0.1;
    bool quiet = false;
    string gname = "";
    rseed = 0;
    GetOpt_pp ops(argc, argv);

    if (ops >> OptionPresent('h', "help") || argc == 1)
        usage();
    //required args
    try {
        ops.exceptions(std::ios::failbit | std::ios::eofbit);
        ops 
                //>> Option('c', "code", codetype)
                //>> Option('n', "n1", n1)
                >> Option('s', "sb1", sb1);
                //>> Option('d', "d1", d1);
                //>> Option('m', "md1", md1);
    } catch (const GetOptEx& e) {
        std::cerr << "Invalid options or format\n";
        usage();
    }
    //optional args
    try {
        ops.exceptions(std::ios::failbit);
        ops >> Option('c', "code", codetype)
		>> Option('n', "n1", n1)
		>>  Option("n2", n2, n1)	
                >> Option("sb2", sb2, sb1)
		>> Option('d', "d1", d1)
                >> Option("d2", d2, d1)
                >> Option("md1", md1, 10)
                >> Option("md2", md2, md1)                
                >> Option('I', "iterations", niter, niter)
                >> Option('w', "graphname", gname, "")
                >> Option('x', "seed", rseed, rseed);
    } catch (const GetOptEx& e) {
        std::cerr << "Invalid options or format\n";
        usage();
    }
    quiet = ops >> OptionPresent('q', "quiet");
    
    
    
    setrandseed(rseed) ;

     
#ifdef USE_PRERAND
     string randfile="/home1/masoud/yearly/2010.bin";
    unsigned int nr=ptrand.openrandfile(randfile.c_str(),32, 25000000);
    if(!nr){
        cerr<<"Could not open file "<<randfile<<"..."<<endl;
        exit(EXIT_FAILURE);
    }
    cerr<<"There are "<<nr<<" 32bit random numbers in "<<randfile<<endl;
#endif     
   


    EBC code(n1, n2, sb1, sb2);

    if (codetype == "ex") {
        if (!code.graph.makeRandomSimpleRegular(d1, d2)) {
            cerr << "Couldn't simplify the random graph" << endl;
            return 1;
        }
    } else if (codetype == "st") {
        code.graph.makeStaircase(d1);
    } else {
        cerr << "Invalid code type --> " << codetype << endl;
        usage();
    }
    
     if (gname != "") {
        code.graph.printAdjacencyLists();
        cout << "writing the graph in graphml format -> " << gname + ".graphml" << " ..." << endl;
        code.graph.writeGraphml(gname + ".graphml");
        cout << "writing the graph in tikz format -> " << gname + ".tikz" << " ..." << endl;
        code.graph.writeTikz(gname + ".tikz");
        return 2;
    }
    
    
    if (ops >> OptionPresent("md")){
        string mdfile;
        ops>>Option("md", mdfile);
      
        if(!code.readMDFile(mdfile)){
            cerr<<"couldn't read the md file, bad name or format"<<endl;
            return -2;
        }    
    }else if (ops >> OptionPresent("md1"))
        code.setRegularMD(md1, md2);
    else if (ops >> OptionPresent("i1"))
        ;
    else{
        cerr<<"regular md should be specified or md file or index to MDFILE"<<endl;
        usage();
    }
    vector< vector<int> > MDs;
     if (ops >> OptionPresent('f', "F")){
        string mdkfile=""; 
        ops >> Option('f', "F", mdkfile);        
        if (!readfile(MDs,mdkfile.c_str())){
                cerr<<"could not open the LIST "<<mdkfile<<endl;
                return -3;
        }        
        if (ops >> OptionPresent( "i1")){
                int mdi1=0; 
                ops >> Option("i1", mdi1,0); 
                int mdi2=0; 
                ops >> Option("i2", mdi2,0); 
                cout<<"Using rows "<<mdi1<<" and "<<mdi2<<endl;
                //code.setRegularMD(MDs[mdi1-1],MDs[mdi2-1]);
                code.setSubcodeInf(MDs[mdi1],MDs[mdi2]);
        }
        
    }
    
    if (ops >> OptionPresent('p', "permutes")){
        int np=5;
        ops >> Option('p', "permutes", np, np);
        code.setUniformRandomMD();
        code.testSystematicProcedure(np);
        return 0;
    }
    if(!md1 || !md2)//for test use md0 to have a random md
        code.setUniformRandomMD();
    
    ////rate = code.designRate();
    sort(code.K1.begin(), code.K1.end());
    sort(code.K2.begin(), code.K2.end());
    int dim=code.rateUpperBound();  
    rate=(double) dim / (sb1 * sb2);
   
     Givaro::GFqDom<int> GF = Givaro::GFqDom<int>(2,8);
     M4RIE::FiniteField *F = (M4RIE::FiniteField*)&GF;
     gf2e *ff=gf2e_init_givgfq(F);

    if (!quiet) {
        //code.makeCheckMatrix();
	//int dim=code.CheckMatrixRank(ff);
	//printf("dim RS=%d\n",dim);
	code.printParams();
        code.printMD();
        code.printSystematic();
        //printf("code=%s, n1=%d, n2=%d, sb1=%d, sb2=%d, d1=%d, d2=%d, md1=%d, md2=%d, rate=%f\n", codetype.c_str(), n1, n2, sb1, sb2, d1, d2, md1, md2, rate);
        printf("\n");
    }


     
 
    ofstream log;
    if (ops >> OptionPresent('o', "out")) {
        string outfile = "";
        ops >> Option('o', "out", outfile, "out.txt");
        if (outfile != "") {
            log.open(outfile.c_str());//,ios::app
            if (!log) {
                cerr << "could not write the output file";
                return -1;
            }
            cout << "Writing results to " << outfile << "..." << endl;
        }
    }
     
    if (ops >> OptionPresent('g', "gap")) {
        double stopcond = 1e-4, reliable = 1e-5;
        
        ops >> Option('g', "gap", stopcond, stopcond);
        ops >> Option('t', "thresh", reliable, reliable);      
        
        printf("binary search to find the capacity gap with err<%e, and stopcond=%e ... \n", reliable, stopcond);
        gapbinsearch(code, rate,stopcond, reliable, niter,log);
        //return 3;
    }


    if (ops >> OptionPresent('e', "erasure")){
        ops >> Option('e', "erasure", erasure, 1 - rate - (1.0 - rate) / 5.0);
        sampleerrorcurve(code,erasure,niter,log);
    }
    
    
    if (ops >> OptionPresent('E', "ER")){     
        string erfile=""; 
        ops >> Option('E', "ER", erfile,"ers.txt"); 
        ifstream ER(erfile.c_str());
        if (!ER){
                cerr<<"could not open the erasure file "<<erfile<<endl;
                return -3;
        }
        vector<double> ers;
        double e;
        while (!ER.eof()){
            ER>>e;
            ers.push_back(e);
        }                      
        ER.close();
        cout<<"read "<<ers.size()<<" erasure value from "<<erfile<<endl;
        for (unsigned int i=0;i<ers.size();i++)
                sampleerrorcurve(code,ers[i],niter,log);        
    }
    

    if(log)
         log.close();
   
    //cout.flush();
  
}