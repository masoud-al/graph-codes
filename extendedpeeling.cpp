
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



#include "getopt_pp.h"

#include "graph.h"
#include "dists.h"
#include "utils.h"

//#include "sse_mathfun.h"

using namespace std;
using namespace GetOpt;




static void usage() {
    cerr << "Usage: gldpc -n <codelength> [-d <leftdegree>] -D <rightdegree> "<<endl;
    //./simcodes -c st -n 10 -s 128 -d 4 -m 5
    //./simcodes -c ex -n 14 -s 256 -d 4 --md1 7 --md2 15 -I 20 -g  
    exit(EXIT_FAILURE);
}



int main(int argc, char* argv[]) {
    int n = 512;
    int d=2,D=8;
    double ov=0.01;
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
                // >>  Option('p', channelparam)    
                >>  Option('o', "overhead", ov, 0.01)
                >>  Option('w', "output", gname, "")
                >>  Option('x', "seed", rseed, rseed);

    } catch (const GetOptEx& e) {
        std::cerr << e.what();
        std::cerr << "Invalid options or format\n";
        usage();
    }
    quiet = ops >> OptionPresent('q', "quiet");
    
    if(!quiet)
        cout<<"Random seed is "<<rseed<<endl;


    setrandseed(rseed);
    
 
    
    //return 0;
    
    BipartiteGraph graph((1+ov)*n,n);
    //graph.makeRandomSimpleRegular(10,10);
    RobustSoliton sol();
    sol.Sample(graph);
    if (gname!="")
        graph.writeSMAT(gname);
    
    cout<<"rank="<<graph.rank()<<endl;
}
