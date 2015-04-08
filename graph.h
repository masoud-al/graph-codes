/* 
 * File:   graph.h
 * Author: masoud
 *
 * Created on February 17, 2012, 11:34 AM
 */

#ifndef GRAPH_H
#define	GRAPH_H


#include <string>

struct Cell {
    Cell *up, *down, *left, *right;
    unsigned int row, column;
    //double llr;
};

class BPHeap {
private:

public:
    BPHeap(unsigned int n1, unsigned int n2);
    BPHeap();
    void init(unsigned int n1,unsigned  int n2);
    Cell* p;
    ~BPHeap();
};

class BipartiteGraph {
public:
    BipartiteGraph(){;}
    BipartiteGraph(unsigned int n1,unsigned  int n2);
    BipartiteGraph(const BipartiteGraph &origin);
    BipartiteGraph(const std::vector< std::vector<unsigned int> >* mat, unsigned int n1, unsigned int n2);
    //setSize(int n1,int n2);
    ~BipartiteGraph(void);
    void reset();
    unsigned int n1, n2;
    unsigned int *deg1, *deg2;
    Cell *v1;
    Cell *v2;
    //Cell* addEdge(int row, int column);
    Cell* p;
    unsigned int CurrentEdge;
    //static BPHeap mem; 
    //static base_srng_type RandomGen;
    unsigned int multiplicity(unsigned int row,unsigned  int column);
    //void removeEdge(Cell *c);
    void printAdjacencyLists(void);
    void printAdjacencyMatrix(void);
    void printDegrees(void);
    bool writeGraphml(std::string fname);
	bool writeLineGraphml(std::string fname);
    bool writeSMAT(std::string fname);
    bool writeTikz(std::string fname);
    bool makeRandomRegular(int d1, int d2);
    void makeComplete();
    bool makeStaircase(int degree);
    bool makeSimple(void);
    bool makeRandomSimpleRegular(int d1, int d2);
    unsigned int nEdges(void);
    unsigned int rank(char mat='G'); // G for generator matrix, H for check matrix

    inline Cell* addEdge(int row, int column) {

#ifdef USE_BOOST_POOL
        Cell *c = (Cell*) mem_pool::malloc();
#else
        //Cell *c = new Cell;
        //Cell *c = p + n1 + n2 + row * n2 + column;
        Cell *c = p+(CurrentEdge++);
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
        
        //llr=0.0;

        return c;
    }

    inline void removeEdge(Cell *c) {
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
};


#endif	/* GRAPH_H */

