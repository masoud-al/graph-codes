#include "ebc.h"
#include <iostream>

using namespace std;

int main(void) {
	EBC code(14, 14, 256, 256);
	//EBC code(4, 4, 4, 4);
	if (!code.graph.makeRandomSimpleRegular(4, 4)) {
		cerr << "Couldn't simplify" << endl;
		return 1;
	}
	//code.graph.makeStaircase(2);
	code.setRegularMD(11,15);
	code.graph.writeGraphml("code.graphml");
	code.graph.printAdjacencyLists();

	cout << "Design rate = " << code.designRate() << endl;
	ErasurePattern ep(code, 0.017);
	//ep.pattern.printAdjacencyLists();
	cout << "#Erasures = " << ep.pattern.nEdges() << endl;
	ep.decode(code);
	cout << "#Remaining erasures = " << ep.pattern.nEdges() << endl;
	//ep.pattern.printAdjacencyLists();
	return 0;
}
