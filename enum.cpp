#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;


int N, lb, ub;
int *a;

void nested_loop(int n, int k);
ofstream a_b_values("a_b_vals.dat",ios::out);  


int main(int argc, char** argv)
{
  if( argc != 4 ) {
    cout << "Invalid use lol" << endl;
    return 1;
  }
  N = atoi( argv[ 1 ] );
  a = new int[N]; //hold the value of the n variables

  lb = atoi( argv[2]);
  ub = atoi(argv[3]);
  int n=N-1;
  nested_loop(n,ub-lb+1);
 return 0; 
}



void nested_loop(int n, int k)
{
    int i,j;
    if(n<0)
    {
	for(j=0;j<N;j++)
	  cout<<a[j]+lb-1<<"\t";
	cout<<endl;
    }
	    
    else
    {
      
	for(i=1;i<=k;i++)
	{
	  a[n]=i;
	  nested_loop(n-1,i);
	}
	  
    }
  
}
