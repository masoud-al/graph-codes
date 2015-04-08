/* 
 * File:   Distributions.h
 * Author: masoud
 *
 * Created on April 19, 2012, 11:06 AM
 */

#ifndef DISTRIBUTIONS_H
#define	DISTRIBUTIONS_H



#include "graph.h"

class DegreeDistribution {
public:
    DegreeDistribution();
    DegreeDistribution(const DegreeDistribution& orig);
    virtual void Sample(BipartiteGraph graph)=0;
    virtual ~DegreeDistribution();
private:

};


class IdealSoliton : public DegreeDistribution{
public:
    IdealSoliton();
    void Sample(BipartiteGraph *graph);
};

class RobustSoliton : public DegreeDistribution{
public:
    unsigned int maxDegree;
    RobustSoliton();
    RobustSoliton(unsigned int max);
    
    void Sample(BipartiteGraph *graph);
};

#endif	/* DISTRIBUTIONS_H */

