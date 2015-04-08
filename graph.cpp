/* 
 * File:   graph.cpp
 * Author: masoud
 * 
 * Created on February 17, 2012, 11:34 AM
 */

#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <queue>
#include <algorithm>
#include <set>


#include <m4ri/m4ri.h>

#include "graph.h"
#include "utils.h"
using namespace std;

BPHeap::BPHeap() {

}

BPHeap::BPHeap(unsigned int n1, unsigned int n2) {
    init(n1, n2);
}

BPHeap::~BPHeap() {
    delete[] p;
}

void BPHeap::init(unsigned int n1, unsigned int n2) {
    p = new Cell[n1 + n2 + n1 * n2];
}

//BPHeap  BipartiteGraph::mem; 

BipartiteGraph::BipartiteGraph(unsigned int n1, unsigned int n2) {
    this->n1 = n1;
    this->n2 = n2;
    p = new Cell[n1 + n2 + n1 * n2];


#ifdef USE_BOOST_POOL1
    v1 = (Cell*) mem_pool::ordered_malloc(n1);
    v2 = (Cell*) mem_pool::ordered_malloc(n2);
#else 
    //v1 = new Cell[n1];
    //v2 = new Cell[n2];
    v1 = p;
    v2 = p + n1;
#endif

    CurrentEdge = n1 + n2;

    deg1 = new unsigned int[n1];
    deg2 = new unsigned int[n2];
    for (unsigned int i = 0; i < n1; i++) {
        deg1[i] = 0;
        v1[i].row = i;
        v1[i].column = -1;
        v1[i].left = v1[i].right = &v1[i];
        v1[i].down = v1[i].up = NULL;
    }
    for (unsigned int j = 0; j < n2; j++) {
        deg2[j] = 0;
        v2[j].row = -1;
        v2[j].column = j;
        v2[j].left = v2[j].right = NULL;
        v2[j].down = v2[j].up = &v2[j];
    }
}

BipartiteGraph::BipartiteGraph(const BipartiteGraph &origin) {
    n1 = origin.n1;
    n2 = origin.n2;
    p = new Cell[n1 + n2 + n1 * n2];
    //v1 = new Cell[n1];
    //v2 = new Cell[n2];
    v1 = p;
    v2 = p + n1;
    CurrentEdge = n1 + n2;
    deg1 = new unsigned int[n1];
    deg2 = new unsigned int[n2];
    for (unsigned int i = 0; i < n1; i++) {
        deg1[i] = 0;
        v1[i].row = i;
        v1[i].column = -1;
        v1[i].left = v1[i].right = &v1[i];
        v1[i].down = v1[i].up = NULL;
    }
    for (unsigned int j = 0; j < n2; j++) {
        deg2[j] = 0;
        v2[j].row = -1;
        v2[j].column = j;
        v2[j].left = v2[j].right = NULL;
        v2[j].down = v2[j].up = &v2[j];
    }
    for (unsigned int i = 0; i < n1; i++)
        for (Cell* iterator = &origin.v1[i]; iterator->right != &origin.v1[i]; iterator = iterator->right)
            addEdge(iterator->right->row, iterator->right->column);
}

BipartiteGraph::BipartiteGraph(const std::vector< std::vector<unsigned int> > *mat, unsigned int _n1, unsigned int _n2){
	n1 = _n1;
    n2 = _n2;
    p = new Cell[n1 + n2 + n1 * n2];
    //v1 = new Cell[n1];
    //v2 = new Cell[n2];
    v1 = p;
    v2 = p + n1;
    CurrentEdge = n1 + n2;
    deg1 = new unsigned int[n1];
    deg2 = new unsigned int[n2];
    for (unsigned int i = 0; i < n1; i++) {
        deg1[i] = 0;
        v1[i].row = i;
        v1[i].column = -1;
        v1[i].left = v1[i].right = &v1[i];
        v1[i].down = v1[i].up = NULL;
    }
    for (unsigned int j = 0; j < n2; j++) {
        deg2[j] = 0;
        v2[j].row = -1;
        v2[j].column = j;
        v2[j].left = v2[j].right = NULL;
        v2[j].down = v2[j].up = &v2[j];
    }
	assert(n1<=mat->size());
    for(unsigned int i = 0; i < n1; i++)
		for (unsigned int j=0;j<mat->at(i).size();j++)
            addEdge(i,mat->at(i)[j]);
}

void BipartiteGraph::reset() {
    for (unsigned int i = 0; i < n1; i++) {
        deg1[i] = 0;
        v1[i].row = i;
        v1[i].column = -1;
        v1[i].left = v1[i].right = &v1[i];
        v1[i].down = v1[i].up = NULL;
    }
    for (unsigned int j = 0; j < n2; j++) {
        deg2[j] = 0;
        v2[j].row = -1;
        v2[j].column = j;
        v2[j].left = v2[j].right = NULL;
        v2[j].down = v2[j].up = &v2[j];
    }
}

BipartiteGraph::~BipartiteGraph(void) {
    for (unsigned int i = 0; i < n1; i++)
        while (v1[i].right != &v1[i])
            removeEdge(v1[i].right);


#ifdef USE_BOOST_POOL1
    mem_pool::ordered_free(v1);
    mem_pool::ordered_free(v2);
#else
    //delete[] v1;
    //delete[] v2;
#endif    

    delete[] deg1;
    delete[] deg2;
    delete[] p;
}

/*
Cell* BipartiteGraph::addEdge(int row, int column) {
    
#ifdef USE_BOOST_POOL
    Cell *c=(Cell*)mem_pool::malloc();
#else
    //Cell *c = new Cell;
    Cell *c =p+n1+n2+row*n2+column;
#endif
    
    c->row = row;
    c->column = column;

    deg1[c->row]++;
    deg2[c->column]++;

    c->left = v1[row].left;
    c->right = &v1[row];
    v1[row].left = c;
    c->left->right = c;

    c->up = v2[column].up;
    c->down = &v2[column];
    v2[column].up = c;
    c->up->down = c;

    return c;
}

void BipartiteGraph::removeEdge(Cell *c) {
    c->left->right = c->right;
    c->right->left = c->left;
    c->up->down = c->down;
    c->down->up = c->up;
    deg1[c->row]--;
    deg2[c->column]--;
#ifdef USE_BOOST_POOL
    mem_pool::free(c);
#else
    //delete c;
#endif
}
 */
void BipartiteGraph::printAdjacencyLists(void) {
    cout << "From the row perspective:";
    for (unsigned int i = 0; i < n1; i++) {
        cout << endl << "Row " << i << ":";
        for (Cell* iterator = &v1[i]; iterator->right != &v1[i]; iterator = iterator->right) {
            cout << " " << iterator->right->column;
            assert(iterator->right->row == i);
        }
    }
    cout << endl << "From the column perspective:";
    for (unsigned int j = 0; j < n2; j++) {
        cout << endl << "Column " << j << ":";
        for (Cell* iterator = &v2[j]; iterator->down != &v2[j]; iterator = iterator->down) {
            cout << " " << iterator->down->row;
            assert(iterator->down->column == j);
        }
    }
    cout << endl;
}

void BipartiteGraph::printAdjacencyMatrix(void) {
    for (unsigned int i = 0; i < n1; i++) {
        cout << endl;
        vector<bool> ed(n2, false);
        for (Cell* iterator = &v1[i]; iterator->right != &v1[i]; iterator = iterator->right) {
            ed[iterator->right->column] = true;
        }
        for (unsigned int j = 0; j < n2; j++)
            cout << ed[j]; //<<" ";
    }
    cout << endl;
}

void BipartiteGraph::printDegrees(void) {
    cout << endl;
    for (unsigned int i = 0; i < n1; i++)
        cout << deg1[i] << " ";
    cout << endl;
    for (unsigned int j = 0; j < n2; j++)
        cout << deg2[j] << " ";

    cout << endl;
}

bool BipartiteGraph::writeSMAT(string fname) {
    ofstream of(fname.c_str());
    if (!of)
        return false;

    of << n1 << " " << n2 << " " << nEdges() << endl;

    for (unsigned int i = 0; i < n1; i++)
        for (Cell* iterator = &v1[i]; iterator->right != &v1[i]; iterator = iterator->right)
            of << i << " " << iterator->right->column << " 1\n";

    of.close();
    return true;
}

bool BipartiteGraph::writeGraphml(string fname) {
    ofstream of(fname.c_str());
    if (!of)
        return false;

    of << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    of << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:y=\"http://www.yworks.com/xml/graphml\" xmlns:yed=\"http://www.yworks.com/xml/yed/3\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns http://www.yworks.com/xml/schema/graphml/1.1/ygraphml.xsd\">\n";
    of << "\t<key for=\"node\" id=\"ng\" yfiles.type=\"nodegraphics\"/>\n";
    of << "\t<graph id=\"G\" edgedefault=\"undirected\">" << endl;
    for (unsigned int i = 0; i < n1; i++) {
        of << "\t <node id=\"r" << i << "\">\n";
        of << "\t  <data key=\"ng\">\n";
        of << "\t   <y:ShapeNode>\n";
        of << "\t 	<y:Geometry height=\"30.0\" width=\"30.0\" x=\"0.0\" y=\"" << i * 60 << "\"/>\n";
        of << "\t 	<y:Fill color=\"#FF0000\" transparent=\"false\"/>\n";
        of << "\t 	<y:NodeLabel alignment=\"center\" >" << i << "</y:NodeLabel>\n";
        of << "\t 	<y:Shape type=\"ellipse\"/>\n";
        of << "\t   </y:ShapeNode>\n";
        of << "\t  </data>\n";
        of << "\t </node>\n";
    }
    for (unsigned int j = 0; j < n2; j++) {
        of << "\t <node id=\"c" << j << "\">\n";
        of << "\t  <data key=\"ng\">\n";
        of << "\t   <y:ShapeNode>\n";
        of << "\t 	<y:Geometry height=\"30.0\" width=\"30.0\" x=\"100.0\" y=\"" << j * 50 << "\"/>\n";
        of << "\t 	<y:Fill color=\"#99CCFF\" transparent=\"false\"/>\n";
        of << "\t 	<y:NodeLabel alignment=\"center\" >" << j << "</y:NodeLabel>\n";
        of << "\t 	<y:Shape type=\"ellipse\"/>\n";
        of << "\t   </y:ShapeNode>\n";
        of << "\t  </data>\n";
        of << "\t </node>\n";
    }
    for (unsigned int i = 0; i < n1; i++)
        for (Cell* iterator = &v1[i]; iterator->right != &v1[i]; iterator = iterator->right)
            of << "\t <edge source=\"r" << i << "\" target=\"c" << iterator->right->column << "\"/>\n";
    of << "\t</graph>" << endl;
    of << "</graphml>";
    of.close();
    return true;
}

bool BipartiteGraph::writeLineGraphml(string fname) {
    ofstream of(fname.c_str());
    if (!of)
        return false;

    of << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    of << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:y=\"http://www.yworks.com/xml/graphml\" xmlns:yed=\"http://www.yworks.com/xml/yed/3\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns http://www.yworks.com/xml/schema/graphml/1.1/ygraphml.xsd\">\n";
    of << "\t<key for=\"node\" id=\"ng\" yfiles.type=\"nodegraphics\"/>\n";
    of << "\t<graph id=\"G\" edgedefault=\"undirected\">" << endl;
    for (unsigned int i = 0; i < n1; i++) {
        of << "\t <node id=\"n" << i << "\">\n";
        of << "\t  <data key=\"ng\">\n";
        of << "\t   <y:ShapeNode>\n";
        of << "\t 	<y:Geometry height=\"30.0\" width=\"30.0\" x=\"0.0\" y=\"" << i * 60 << "\"/>\n";
        of << "\t 	<y:Fill color=\"#FF0000\" transparent=\"false\"/>\n";
        of << "\t 	<y:NodeLabel alignment=\"center\" >" << i << "</y:NodeLabel>\n";
        of << "\t 	<y:Shape type=\"ellipse\"/>\n";
        of << "\t   </y:ShapeNode>\n";
        of << "\t  </data>\n";
        of << "\t </node>\n";
    }
   /* for (unsigned int j = 0; j < n2; j++) {
        of << "\t <node id=\"c" << j << "\">\n";
        of << "\t  <data key=\"ng\">\n";
        of << "\t   <y:ShapeNode>\n";
        of << "\t 	<y:Geometry height=\"30.0\" width=\"30.0\" x=\"100.0\" y=\"" << j * 50 << "\"/>\n";
        of << "\t 	<y:Fill color=\"#99CCFF\" transparent=\"false\"/>\n";
        of << "\t 	<y:NodeLabel alignment=\"center\" >" << j << "</y:NodeLabel>\n";
        of << "\t 	<y:Shape type=\"ellipse\"/>\n";
        of << "\t   </y:ShapeNode>\n";
        of << "\t  </data>\n";
        of << "\t </node>\n";
    }*/

	Bitset* edgeset=new Bitset[n1];
    for (unsigned int i = 0; i < n1; i++){
		EmptySet(edgeset[i]);
        for (Cell* iterator = &v1[i]; iterator->right != &v1[i]; iterator = iterator->right)
			SetBit(edgeset[i],iterator->right->column);
	}


	for (unsigned int i = 0; i < n1; i++)
		for (unsigned int j = i+1; j < n1; j++)
			if (!IsEmptySet(Intersection(edgeset[i],edgeset[j])))
				of << "\t <edge source=\"n" << i << "\" target=\"n" << j << "\"/>\n";
            
    of << "\t</graph>" << endl;
    of << "</graphml>";
    of.close();
    return true;
}

bool BipartiteGraph::writeTikz(string fname) {
    ofstream of(fname.c_str());
    if (!of)
        return false;

    of << "\\begin{tikzpicture}[]" << endl;
    for (unsigned int i = 0; i < n1; i++) {
        of << "\\node[shape=circle,draw,fill=blue!20]    (r" << i << ") at (" << i << ",5) {$" << i << "$};\n";
    }
    for (unsigned int j = 0; j < n2; j++) {
        of << "\\node[shape=rectangle,draw,fill=red!20]    (c" << j << ") at (" << j + (n1 - n2) / 2.0 << ",0) {$" << j << "$};\n";
    }
    for (unsigned int i = 0; i < n1; i++)
        for (Cell* iterator = &v1[i]; iterator->right != &v1[i]; iterator = iterator->right)
            of << "\\draw[-,line width=1pt,color=black] (r" << i << ") edge (c" << iterator->right->column << ");\n";
    //for edge label
    //of<<"\draw[-,line width=1pt,color=black] (a"<< i<<") edge[auto] node[auto]{\tiny x1} (b"<<iterator->right->column << ");\n";	
    of << "\\end{tikzpicture}";
    of.close();
    return true;
}

bool BipartiteGraph::makeRandomRegular(int d1, int d2) {
    unsigned int nEdge = d1 * n1;
    if (nEdge != d2 * n2)
        return false;
    int *socket = new int[nEdge];
    for (unsigned int j = 0; j < n2; j++)
        for (int k = 0; k < d2; k++)
            socket[j * d2 + k] = j;
    permute(socket, nEdge);

    for (unsigned int i = 0; i < n1; i++)
        for (int k = 0; k < d1; k++) {
            addEdge(i, socket[i * d1 + k]);

        }
    delete[] socket;
    return true;
}

bool BipartiteGraph::makeStaircase(int degree) {
    if (n1 != n2)
        return false;
    for (unsigned int i = 0; i < n1; i++)
        for (int j = 0; j < degree; j++)
            addEdge(i, (i + j) % n1);
    return true;
}

void BipartiteGraph::makeComplete() {
    for (unsigned int i = 0; i < n1; i++)
        for (unsigned int j = 0; j < n2; j++)
            addEdge(i, j);
}

unsigned int BipartiteGraph::multiplicity(unsigned int row, unsigned int column) {
    unsigned int counter = 0;
    for (Cell* iterator = &v1[row]; iterator->right != &v1[row]; iterator = iterator->right)
        counter += (iterator->right->column == column);
    return counter;
}

bool BipartiteGraph::makeSimple(void) {
    vector<Cell *> edges;
    for (unsigned int i = 0; i < n1; i++)
        for (Cell* iterator = &v1[i]; iterator->right != &v1[i]; iterator = iterator->right)
            edges.push_back(iterator->right);
    for (unsigned e = 0; e < edges.size(); e++)
        if (multiplicity(edges[e]->row, edges[e]->column) > 1) {
            bool resolved = false;
            for (unsigned nTries = 0; !resolved && nTries < edges.size() * 1000; nTries++) {
                int f = rand() % edges.size();
                if (edges[f]->column != edges[e]->column || edges[f]->row != edges[e]->row)
                    if (multiplicity(edges[f]->row, edges[e]->column) == 0 && multiplicity(edges[e]->row, edges[f]->column) == 0) {
                        Cell *edge1 = addEdge(edges[f]->row, edges[e]->column);
                        Cell *edge2 = addEdge(edges[e]->row, edges[f]->column);
                        removeEdge(edges[e]);
                        removeEdge(edges[f]);
                        edges[e] = edge1;
                        edges[f] = edge2;
                        resolved = true;
                    }
            }
            if (!resolved)
                return false;
        }
    return true;
}

bool BipartiteGraph::makeRandomSimpleRegular(int d1, int d2) {
    if (!makeRandomRegular(d1, d2))
        return false;
    return makeSimple();
}

unsigned int BipartiteGraph::nEdges() {
    unsigned int result = 0;
    for (unsigned int i = 0; i < n1; i++)
        result += deg1[i];
    return result;
}


unsigned int BipartiteGraph::rank(char mat) {

    // left nodes=vars=columns and right nodes=outputs=row
    mzd_t *A = mzd_init(n2, n1);
    for (unsigned int i = 0; i < n1; i++)
        for (Cell* iterator = &v1[i]; iterator->right != &v1[i]; iterator = iterator->right)
            mzd_write_bit(A, iterator->right->column, i, true);

    unsigned int r = mzd_echelonize(A, 1);
	mzd_free(A);
    return r;
}
