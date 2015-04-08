#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <givaro/givgfq.h>
#include <m4ri/m4ri.h>
#include <m4rie/m4rie.h>
#include <m4rie/finite_field_givaro.h>

using namespace std;
using namespace Givaro;


/*Conway polynomials
2,1 = X + 1     
2,2 = X2 + X + 1     
2,3 = X3 + X + 1     
2,4 = X4 + X + 1     
2,5 = X5 + X2 + 1     
2,6 = X6 + X4 + X3 + X + 1     
2,7 = X7 + X + 1     
2,8 = X8 + X4 + X3 + X2 + 1     
2,9 = X9 + X4 + 1     
2,10 = X10 + X6 + X5 + X3 + X2 + X + 1     
2,11 = X11 + X2 + 1     
2,12 = X12 + X7 + X6 + X5 + X3 + X + 1     
2,13 = X13 + X4 + X3 + X + 1     
2,14 = X14 + X7 + X5 + X3 + 1     
2,15 = X15 + X5 + X4 + X2 + 1     
2,16 = X16 + X5 + X3 + X2 + 1     
*/



int main(int argc, char** argv){
  if(argc!=4){
    printf("./rsgen n k e\n");
    printf("RS code : [n,k,n-k+1]_2^e\n");
    return 1;
  }
  int n=strtol(argv[1],NULL,0);
  int k=strtol(argv[2],NULL,0);
  int e=strtol(argv[3],NULL,0);
  Givaro::GFqDom<int> GF2e (2,e);
  Givaro::GFqDom<int>::Element gen,a;
  const Givaro::GFqDom<int>::Element unity(1);
  GF2e.generator(gen);
  GF2e.write(std::cout <<
                "In this field, the generator used is (in 2-adic): ", gen)
                          << std::endl
                          << " whose internal representation is "
                          << gen << std::endl;

  std::cout << "Irreducible (in 2-adic): "
              << GF2e.irreducible() << std::endl;
  
	
  printf("RS code -> [%d,%d,%d]_2^%d\n",n,k,n-k+1,e);
  GFqDom<int> GF = Givaro::GFqDom<int>(2,e);
  M4RIE::FiniteField *F = (M4RIE::FiniteField*)&GF;
  gf2e *ff=gf2e_init_givgfq(F);
  mzed_t*  G = mzed_init(ff,k,n);
  //mzed_randomize(G);
  //vector< GFqDom<int>::Element> alpha,row;
  vector< int> alpha,row;
  //choose distinc alphas
  for(int j=0;j<n;j++){
    //select randomly
    /*GF2e.init(a,(Integer)213); 
    alpha.push_back(a);
    row.push_back(unity);
    cout<<alpha[j]<<endl;
    mzed_write_elem(G,0,j,unity); //set first row to all one vector*/
    alpha.push_back(j);
    row.push_back(1);
    mzed_write_elem(G,0,j,1);
  }
  
  for(int i=1;i<k;i++)
    for(int j=0;j<n;j++){
      //GF2e.mul(row[j], row[j] , alpha[j]);
      row[j]=ff->mul[row[j]][alpha[j]];
      mzed_write_elem(G,i,j,row[j]);
    }
  mzed_print(G);
  
  
  mzed_t* T=mzed_copy(NULL,G);
  int rank=mzed_echelonize(T,true);
  assert(rank==k);//
  printf("\n");
  mzed_print(T);
  
  mzed_t*  H = mzed_init(ff,n-rank,n);
  for(int i=0;i<n-rank;i++){
    mzed_write_elem(H,i,rank+i,unity);
    for(int j=0;j<rank;j++)
      mzed_write_elem(H,i,j,mzed_read_elem(T,j,rank+i));
  }
  printf("\n");
  mzed_print(H);
  
  mzed_free(G);
  mzed_free(T);
  mzed_free(H);
  
  return 0;
}
