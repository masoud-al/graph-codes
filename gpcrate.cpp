#include "ebc.h"
#include <iostream>

using namespace std;

int main(void) {
	EBC code(1, 1, 8, 8);
	//EBC code(4, 4, 4, 4);
	code.graph.makeStaircase(1);
	//code.setRegularMD(5,5);
        code.setUniformRandomMD();
        code.printMD();
	//code.graph.writeGraphml("code.graphml");
	//code.graph.printAdjacencyLists();
	//cout << "Wrong design rate = " << code.designRate() << endl;
	cout << "Upperbound on the design rate = " << code.rateUpperBound() << endl;
	code.printSystematic();
	ErasurePattern ep(code, 0.017);
	//ep.pattern.printAdjacencyLists();
	cout << "#Erasures = " << ep.pattern.nEdges() << endl;
	ep.decode(code);
	cout << "#Remaining erasures = " << ep.pattern.nEdges() << endl;
	//ep.pattern.printAdjacencyLists();
	return 0;
}
