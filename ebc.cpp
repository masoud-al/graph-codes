#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <queue>
#include <algorithm>
#include <set>

#include <m4ri/m4ri.h>
#include <m4rie/m4rie.h>
#include <m4rie/finite_field_givaro.h>

#include "ebc.h"
#include "utils.h"


using namespace std;


EBC::EBC(int nb1, int nb2, int sb1, int sb2) : graph(nb1, nb2) {
    this->sb1 = sb1;
    this->sb2 = sb2;
    md1 = new int[nb1*sb1];
    md2 = new int[nb2*sb2];
}

EBC::~EBC(void) {
    delete[] md1;
    delete[] md2;
}

void EBC::setRegularMD(int md1, int md2) {
    for (int i = 0; i < graph.n1 * sb1; i++)
        this->md1[i] = md1;
    for (int j = 0; j < graph.n2 * sb2; j++)
        this->md2[j] = md2;
    setSubcodeInf();
}

void EBC::setRegularMD(vector<int>& MD1,vector<int>& MD2) {
    for (int i = 0; i < graph.n1 * sb1; i++)
        this->md1[i] = MD1[i];
    for (int j = 0; j < graph.n2 * sb2; j++)
        this->md2[j] = MD2[j];
    setSubcodeInf();
}

void EBC::setUniformRandomMD() {
    int a1=2;int b1=sb2-2;
    for (int i = 0; i < graph.n1 *sb1; i++)
        this->md1[i] = a1 + (double(rand()) / RAND_MAX)*(b1 - a1);
    int a2=2;int b2=sb1-2;
    for (int j = 0; j < graph.n2 * sb2; j++)
        this->md2[j] = a2 + (double(rand()) / RAND_MAX)*(b2 - a2);
    setSubcodeInf();
}

bool EBC::readMDFile(string fname){
    ifstream mdf(fname.c_str());
    if (!mdf)
        return false;
    //there should be two rows of numbers
    for (int i = 0; i < graph.n1 *sb1; i++)
        mdf>>md1[i];
    //mdf>>endl;
    for (int j = 0; j < graph.n2 * sb2; j++)
        mdf>>md2[j];
    mdf.close();
    setSubcodeInf();
    return true;
}

void EBC::setSubcodeInf(){
    //asuming 1x1 block and MDS codes
    assert(graph.n1 == 1 && graph.n2 == 1);
    K1.clear();K2.clear();
    for (int i = 0; i < sb1; i++) {
        int k = sb2 - md1[i] + 1;
        assert(k >= 0);
        K1.push_back(k);
    }
    for (int j = 0; j < sb2; j++) {
        int k = sb1 - md2[j] + 1;
        assert(k >= 0);
        K2.push_back(k);
    }
    systematic.clear();
    for (int i = 0; i < sb1; i++) {
        vector<char> arow;
        for (int j = 0; j < sb2; j++)
            arow.push_back('_');
        systematic.push_back(arow);
    }
}

void EBC::setSubcodeInf(vector<int> a,vector<int> b){
    //asuming 1x1 block and MDS codes
    assert(graph.n1 == 1 && graph.n2 == 1);
    K1=a;K2=b;
    for (int i = 0; i < sb1; i++) {
        md1[i] = sb2 - K1[i] + 1;
        assert(md1[i] >= 2);
    }
    for (int j = 0; j < sb2; j++) {
        md2[j] = sb1 - K2[j]  + 1;
        assert(md2[j] >= 2);        
    }
    systematic.clear();
    for (int i = 0; i < sb1; i++) {
        vector<char> arow;
        for (int j = 0; j < sb2; j++)
            arow.push_back('_');
        systematic.push_back(arow);
    }
}

void EBC::printMD() {
    cout<<"MD1 & MD2:"<<endl;
    for (int i = 0; i < graph.n1 * sb1; i++)
        cout<<" "<<md1[i];
    cout<<"\t\t";
    for (unsigned int i = 0; i < K1.size(); i++)
        cout<<" "<<K1[i];
    
    cout<<endl;
    for (int j = 0; j < graph.n2 * sb2; j++)
        cout<<" "<<md2[j];
    cout<<"\t\t";
    for (unsigned int j = 0; j < K1.size(); j++)
        cout<<" "<<K2[j];
    
    cout<<endl;
    
}

//wrong
double EBC::designRate(void) {
    codeLength = 0;
    nConstraints = 0;
    for (int i = 0; i < graph.n1; i++) {
        codeLength += graph.deg1[i] * sb1 * sb2;
        nConstraints += (md1[i] - 1) * sb1;
    }
    for (int j = 0; j < graph.n2; j++)
        nConstraints += (md2[j] - 1) * sb2;
    return double(codeLength - nConstraints) / codeLength;
}

void EBC::printSystematic() {
    //asuming 1x1 block
    assert(graph.n1 == 1 && graph.n2 == 1);
    cout<<endl;
    int k;
    for (unsigned int i = 0; i < systematic.size(); i++) {
        k=0;
        for (unsigned int j = 0; j < systematic[i].size(); j++){
            if(systematic[i][j]=='x' || systematic[i][j]=='c')
                k++;
            cout << systematic[i][j]<<" ";
        }
        printf(" %d,%d\n",K1[i],k);
    }
    vector<int> I;
    for (unsigned int j = 0; j < systematic[0].size(); j++){
        k=0;
        for (unsigned int i = 0; i < systematic.size(); i++){
           if(systematic[i][j]=='x' || systematic[i][j]=='r')
                k++;
        }
        I.push_back(k);
        cout<<K2[j] <<" ";
    }
    cout<<endl;
    for (unsigned int j = 0; j < systematic[0].size(); j++)
        cout<<I[j] <<" ";
    cout<<endl;
}

unsigned int EBC::nCheckOnCheck(){
    unsigned int cc=0;
     for (unsigned int i = 0; i < systematic.size(); i++) 
        for (unsigned int j = 0; j < systematic[i].size(); j++)
            if(systematic[i][j]=='_')
                cc++;
              
    return cc;    
}

void EBC::testSystematicProcedure(int nperm){
    sort(K1.begin(), K1.end());
    sort(K2.begin(), K2.end());
    //sort(colinf.begin(), colinf.end(), std::greater<int>());
    rateUpperBound();
    printSystematic();
    printParams();
    //return;
    for (int p=0;p<nperm;p++){
        permute(K1);
        permute(K2);
        rateUpperBound();
        printSystematic();
        printParams();
    }
}

void EBC::makeCheckMatrix(){
    vector< vector<char> > H;
    H.clear();
    int nupconst=0;
    for ( int r = 0; r < sb1; r++)
        nupconst+=sb2-K1[r];
    for ( int c = 0; c < sb2; c++)  
        nupconst+=sb1-K2[c];
    for(int i = 0; i < nupconst; i++){
        vector<char> tmp;
        for ( int j = 0; j < sb1*sb2; j++)
                tmp.push_back('0');
        H.push_back(tmp);
    }
    
    mzd_t*  HH = mzd_init(nupconst,sb1*sb2);
    
    int s=0;
    for ( int r = 0; r < sb1; r++) //row codes
        for ( int i = 0; i < sb2-K1[r]; i++,s++)
            for ( int j = 0; j < sb2; j++){
                H[s][r*sb2+j]='x';
                mzd_write_bit(HH,s,r*sb2+j,1);
            }
        
        
    for ( int c = 0; c < sb2; c++) //row codes
        for ( int i = 0; i < sb1-K2[c]; i++,s++)
            for ( int j = 0; j < sb1; j++){
                H[s][c+j*sb2]='x';    
                mzd_write_bit(HH,s,c+j*sb2,1);               
            }
     
    /*cout<<endl;   
     for ( int i = 0; i < nupconst; i++){
            for ( int j = 0; j < sb1*sb2; j++)
                cout<<H[i][j]<<" ";
            cout<<endl;
    }    
    cout<<endl;*/
    int urank=mzd_echelonize_m4ri(HH,1,0);
    //int urank= mzd_echelonize_pluq(HH,1);
    
    //PrintMatrix(HH); 
    printf("Full Parity matrix = [%d,%d] ,Current rank=%d , rate=%f\n",nupconst,sb1*sb2, urank,1.0-(double)urank/(sb1*sb2));
    
    mzd_free(HH);
}

mzed_t* G2H(mzed_t* G,int n, int k){
  mzed_t* T=mzed_copy(NULL,G);
  int rank=mzed_echelonize(T,true);
  assert(rank==k);//
  //printf("\n");
  //mzed_print(T);
  
  mzed_t*  H = mzed_init(G->finite_field,n-rank,n);
  for(int i=0;i<n-rank;i++){
    mzed_write_elem(H,i,rank+i,1);
    for(int j=0;j<rank;j++)
      mzed_write_elem(H,i,j,mzed_read_elem(T,j,rank+i));
  }
  //printf("\n");
  //mzed_print(H);

  mzed_free(T); 
  return H;
}

mzed_t* RSGenerator(int n,int k,gf2e *field,char x){
  mzed_t*  G = mzed_init(field,k,n);
  //mzed_randomize(G);
  //vector< GFqDom<int>::Element> alpha,row;
  vector< int> alpha,row;
  //choose distinc alphas
  for(int i=1;i<256;i++)
     //alpha.push_back(field->pow_gen[i]);
     alpha.push_back(i);
  
   /*if(x=='r')
        srand(1460);
   else 
       srand(64335);
   permute(alpha);*/
  
  
  for(int j=0;j<n;j++){
    //select randomly
    /*GF2e.init(a,(Integer)213); 
    alpha.push_back(a);
    row.push_back(unity);
    cout<<alpha[j]<<endl;
    mzed_write_elem(G,0,j,unity); //set first row to all one vector*/
    //alpha.push_back(j+2);
    row.push_back(1);
    mzed_write_elem(G,0,j,1);
    
  }
  for(int i=1;i<k;i++)
    for(int j=0;j<n;j++){
      //GF2e.mul(row[j], row[j] , alpha[j]);
      row[j]=field->mul[row[j]][alpha[j]];
      mzed_write_elem(G,i,j,row[j]);
    }
  //mzed_print(G);
  return G;  
}

int EBC::CheckMatrixRank(void *f){
    int nupconst=0;
    for ( int r = 0; r < sb1; r++)
        nupconst+=sb2-K1[r];
    for ( int c = 0; c < sb2; c++)  
        nupconst+=sb1-K2[c];

    gf2e *ff=(gf2e*)f;
    mzed_t *HH = mzed_init(ff,nupconst,sb1*sb2);
    
    int s=0;
    for ( int r = 0; r < sb1; r++) //row codes
    {
        mzed_t* G=RSGenerator(sb2,K1[r],ff,'r');
	mzed_t* H=G2H(G,sb2,K1[r]);
	//printf("------------\n");mzed_print(H);
	//mzed_t* W=mzed_init_window(HH,s,r*sb2,s+sb2-K1[r],(r+1)*sb2);
	//printf("%d x %d\n",H->nrows-W->nrows,H->ncols-W->ncols);
	//mzed_copy(W,H);
	//printf("\n");mzed_print(W);
	//mzed_free_window(W);
        for ( int i = 0; i < sb2-K1[r]; i++,s++)
            for ( int j = 0; j < sb2; j++)
                mzed_write_elem(HH,s,r*sb2+j,mzed_read_elem(H,i,j));
	//s+=sb2-K1[r];
	mzed_free(G);
	mzed_free(H);
    }        
        
        
    for ( int c = 0; c < sb2; c++){ //col codes
	mzed_t* G=RSGenerator(sb1,K2[c],ff,'c');
	mzed_t* H=G2H(G,sb1,K2[c]);
        for ( int i = 0; i < sb1-K2[c]; i++,s++)
            for ( int j = 0; j < sb1; j++){
                mzed_write_elem(HH,s,c+j*sb2,mzed_read_elem(H,i,j));               
            }
        mzed_free(G);
	mzed_free(H);    
    }
    
    //mzed_print(HH);
    int urank=mzed_echelonize(HH,true);
    //printf("------------\n");mzed_print(HH);
    
    
    printf("Full Parity matrix = [%d,%d] ,Current rank=%d, nInfSymbols=%d , rate=%f\n",nupconst,sb1*sb2, urank,sb1*sb2-urank,1.0-(double)urank/(sb1*sb2));
    
    mzed_free(HH);
    return sb1*sb2-urank;
}

int EBC::rateUpperBound(void) {
    //asuming 1x1 block and MDS codes
    assert(graph.n1 == 1 && graph.n2 == 1);
    for (int i = 0; i < sb1; i++) {
        for (int j = 0; j < sb2; j++)
            systematic[i][j] = '_';
    }
    vector<int> rowinf = K1;
    vector<int> colinf = K2;
    queue< pair <int, char> > q;
    BipartiteGraph g(sb1, sb2);
    g.makeComplete();
    //g.printAdjacencyLists();
    for (int i = 0; i < sb1; i++)
        if (rowinf[i] == 0)
            q.push(make_pair(i, 'r'));
    for (int j = 0; j < sb2; j++)
        if (colinf[j] == 0)
            q.push(make_pair(j, 'c'));
    int K = 0;
    //int CC = 0;
    int nrow = g.n1;
    int ncol = g.n2;
    while (nrow + ncol) {
        if (!q.empty()) {
            /*printf("[%d,%d]\n", nrow, ncol);
            printf("-----------\n");
            for (unsigned int i = 0; i < rowinf.size(); i++)
                printf("%d, ", rowinf[i]);
            printf("\n");
            for (unsigned int j = 0; j < colinf.size(); j++)
                printf("%d, ", colinf[j]);
            printf("\n------------\n");

            printSystematic();*/


            //remove zero row or column 
            pair< int, char> p = q.front();
            q.pop();
            if (p.second == 'r') {//remove a row 
                nrow--;
                int i = p.first;
                rowinf[i] = 0;
                while (g.v1[i].right != &(g.v1[i])) {
                    int j = g.v1[i].right->column;
                    g.removeEdge(g.v1[i].right);
                    //if(systematic[i][j]!='x') systematic[i][j] = 'r';
                    colinf[j]--;
                    if (colinf[j] == 0)
                        q.push(make_pair(j, 'c'));
                }
            } else {//remove a column
                ncol--;
                int j = p.first;
                colinf[j] = 0;
                while (g.v2[j].down != &(g.v2[j])) {
                    int i = g.v2[j].down->row;
                    g.removeEdge(g.v2[j].down);
                    rowinf[i]--;
                    //if(systematic[i][j]!='x')  
                    if (rowinf[i] >= 0)
                        systematic[i][j] = 'c';
                    if (rowinf[i] == 0)
                        q.push(make_pair(i, 'r'));
                }
            }
            continue;
        }

        if (nrow) { //choose the first row
            Cell* iterator;
            int j, i = 0;
            while (rowinf[i] == 0) i++;
            K += rowinf[g.v1[i].right->row];
            for (iterator = &(g.v1[i]), j = 0; iterator->right != &g.v1[i]; j++, iterator = iterator->right) {
                if (j < rowinf[i])
                    systematic[i][iterator->right->column] = 'x';
                else
                    systematic[i][iterator->right->column] = 'r';

            }
            q.push(make_pair(i, 'r')); //g.v1[i].right->row);
            //rowinf[i]=0;
        } /*else {//choose the first column
            Cell* iterator;
            int i,j=0;
            while(colinf[j]==0) j++;
            K+=colinf[g.v2[j].down->column];
            for (iterator = &(g.v2[j]), i = 0;iterator->down != &g.v2[j] ; i++, iterator = iterator->down){
                if(i < colinf[j])
                    systematic[iterator->down->row][j] = 'x';
                else
                    systematic[iterator->down->row][j] = 'c'; 
            }
            q.push(make_pair(j,'c'));
            //colinf[j]=0;
        }*/

    }
    /*
    printf("-----------\n");
    for (unsigned int i = 0; i < rowinf.size(); i++)
        printf("%d, ", rowinf[i]);
    printf("\n");
    for (unsigned int j = 0; j < colinf.size(); j++)
        printf("%d, ", colinf[j]);
    printf("\n------------\n");
     */

    nInfSymbols = K;
    codeLength = sb1 * sb2 * graph.n1 * graph.n2;
    return K;
}
/*double EBC::rateUpperBound(void) {
    //asuming 1x1 block and MDS codes
    assert(graph.n1 == 1 && graph.n2 == 1);
    for (int i = 0; i < sb1; i++) {
        for (int j = 0; j < sb2; j++)  
                systematic[i][j]='_';
    }
    vector<int> rowinf=K1;
    vector<int> colinf=K2;
    queue<int> q1, q2;
    BipartiteGraph g(sb1, sb2);
    g.makeComplete();
    //g.printAdjacencyLists();
    for (int i = 0; i < sb1; i++)
        if (rowinf[i] == 0)
            q1.push(i);
    for (int j = 0; j < sb2; j++)
        if (colinf[j] == 0)
            q2.push(j);
    int K = 0;
    int nrow = g.n1;
    int ncol = g.n2;
    while (nrow + ncol) {
        //printf("[%d,%d]\n",nrow,ncol);
        if (!q1.empty() || !q2.empty()) {
            //remove zero row or column 
            if (!q1.empty()) {//remove a row 
                nrow--;
                int i = q1.front();
                q1.pop();
                while (g.v1[i].right != &(g.v1[i])) {
                    int j = g.v1[i].right->column;
                    g.removeEdge(g.v1[i].right);
                    colinf[j]--;
                    if (colinf[j] == 0)
                        q2.push(j);
                }
            } else {//remove a column
                ncol--;
                int j = q2.front();
                q2.pop();                
                while (g.v2[j].down != &(g.v2[j])) {
                    int i = g.v2[j].down->row;
                    g.removeEdge(g.v2[j].down);
                    rowinf[i]--;
                    systematic[i][j] = 'c';
                    if (rowinf[i] == 0)
                        q1.push(i);
                }
            }
            continue;
        }

        if (nrow) { //choose the first row
            Cell* iterator;
            int j,i=0;
            while(rowinf[i]==0) i++;            
            K+=rowinf[g.v1[i].right->row];
            for (iterator = &(g.v1[i]), j = 0; iterator->right != &g.v1[i]; j++, iterator = iterator->right){
                if(j < rowinf[i])
                    systematic[i][iterator->right->column] = 'x';
                else
                    systematic[i][iterator->right->column] = 'r';
      
            }
            q1.push(i);//g.v1[i].right->row);
            rowinf[i]=0;
        } else {//choose the first column
            Cell* iterator;
            int i,j=0;
            while(colinf[j]==0) j++;
            K+=colinf[g.v2[j].down->column];
            for (iterator = &(g.v2[j]), i = 0;iterator->down != &g.v2[j] ; i++, iterator = iterator->down){
                if(i < colinf[j])
                    systematic[iterator->down->row][j] = 'x';
                //else
                //    systematic[iterator->down->row][j] = 'c'; 
            }
            q2.push(j);
            colinf[j]=0;
        }
    }
    nInfSymbols=K;
    codeLength=sb1*sb2*graph.n1*graph.n2;
    return (double) K / (sb1 * sb2);
}*/

void EBC::printParams() {
    printf("%d, %d, %d, %d, N=%d, K=%d, CC=%d, R<=%f\n", graph.n1, graph.n2, sb1, sb2, codeLength, nInfSymbols,nCheckOnCheck(),nInfSymbols/(double)codeLength);
}

ErasurePattern::ErasurePattern(const EBC& code): pattern(code.graph.n1 * code.sb1, code.graph.n2 * code.sb2){
    
}

ErasurePattern::ErasurePattern(const EBC& code, double erasureProb) : pattern(code.graph.n1 * code.sb1, code.graph.n2 * code.sb2) {
    //this->code = code;
    //GenTypeBernoli flip(RandomGen, boost::bernoulli_distribution<>(erasureProb));
    for (int i = 0; i < code.graph.n1; i++)
        for (Cell* iterator = &(code.graph.v1[i]); iterator->right != &(code.graph.v1[i]); iterator = iterator->right)
            for (int k1 = 0; k1 < code.sb1; k1++)
                for (int k2 = 0; k2 < code.sb2; k2++)
                     if(Bernouli(erasureProb))
                        pattern.addEdge(iterator->right->row * code.sb1 + k1, iterator->right->column * code.sb2 + k2);
                
}

ErasurePattern::ErasurePattern(const EBC& code,  std::string fname) : pattern(code.graph.n1 * code.sb1, code.graph.n2 * code.sb2) {
    //this->code = code;
    ifstream efile(fname.c_str());
    
    for (int i = 0; i < code.sb1; i++)
         for (int j = 0; j < code.sb2; j++)
	 {
	   int v=-1;
	   efile>>v;
	   if(v==1)
                 pattern.addEdge(i,j);
		 
	 }
    efile.close();
}

unsigned int ErasurePattern::instance(const EBC& code, double erasureProb){
    unsigned int ner=0;
    pattern.reset();
    for (int i = 0; i < code.graph.n1; i++)
        for (Cell* iterator = &(code.graph.v1[i]); iterator->right != &(code.graph.v1[i]); iterator = iterator->right)
            for (int k1 = 0; k1 < code.sb1; k1++)
                for (int k2 = 0; k2 < code.sb2; k2++){
                     if(Bernouli(erasureProb))
                     {
                        ner++; 
                        pattern.addEdge(iterator->right->row * code.sb1 + k1, iterator->right->column * code.sb2 + k2);
                     }
                }
    return ner;
}



void ErasurePattern::decode(const EBC& code) {
    assert(code.graph.n1 == 1 && code.graph.n2 == 1);
    queue<int> q1, q2;
    int *mark1 = new int[pattern.n1], *mark2 = new int[pattern.n2];
    for (int i = 0; i < pattern.n1; i++)
        if ((mark1[i] = (pattern.deg1[i] < code.md1[i])))
            q1.push(i);
    for (int j = 0; j < pattern.n2; j++)
        if ((mark2[j] = (pattern.deg2[j] < code.md2[j])))
            q2.push(j);
    while (!q1.empty() || !q2.empty()) {

        if (!q1.empty()) {
            int i = q1.front();
            q1.pop();
            while (pattern.v1[i].right != &(pattern.v1[i])) {
                int j = pattern.v1[i].right->column;
                pattern.removeEdge(pattern.v1[i].right);
                if (!mark2[j] && pattern.deg2[j] < code.md2[j]) {
                    mark2[j] = 1;
                    q2.push(j);
                }
            }
        } else {
            int j = q2.front();
            q2.pop();
            while (pattern.v2[j].down != &(pattern.v2[j])) {
                int i = pattern.v2[j].down->row;
                pattern.removeEdge(pattern.v2[j].down);
                if (!mark1[i] && pattern.deg1[i] < code.md1[i ]) {
                    mark1[i] = 1;
                    q1.push(i);
                }
            }
        }
        //pattern.printAdjacencyMatrix();
	//pattern.printDegrees();
    }
    delete[] mark1;
    delete[] mark2;
}

/*void ErasurePattern::decode(const EBC& code) {
    queue<int> q1, q2;
    int *mark1 = new int[pattern.n1], *mark2 = new int[pattern.n2];
    for (int i = 0; i < pattern.n1; i++)
        if ((mark1[i] = (pattern.deg1[i] < code.md1[i / code.sb1])))
            q1.push(i);
    for (int j = 0; j < pattern.n2; j++)
        if ((mark2[j] = (pattern.deg2[j] < code.md2[j / code.sb2])))
            q2.push(j);
    while (!q1.empty() || !q2.empty()) {
        if (!q1.empty()) {
            int i = q1.front();
            q1.pop();
            while (pattern.v1[i].right != &(pattern.v1[i])) {
                int j = pattern.v1[i].right->column;
                pattern.removeEdge(pattern.v1[i].right);
                if (!mark2[j] && pattern.deg2[j] < code.md2[j / code.sb2]) {
                    mark2[j] = 1;
                    q2.push(j);
                }
            }
        } else {
            int j = q2.front();
            q2.pop();
            while (pattern.v2[j].down != &(pattern.v2[j])) {
                int i = pattern.v2[j].down->row;
                pattern.removeEdge(pattern.v2[j].down);
                if (!mark1[i] && pattern.deg1[i] < code.md1[i / code.sb1]) {
                    mark1[i] = 1;
                    q1.push(i);
                }
            }
        }
    }
    delete[] mark1;
    delete[] mark2;
}*/