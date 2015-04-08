/* 
 * File:   Distributions.cpp
 * Author: masoud
 * 
 * Created on April 19, 2012, 11:06 AM
 */

#include "dists.h"

DegreeDistribution::DegreeDistribution() {
}

DegreeDistribution::DegreeDistribution(const DegreeDistribution& orig) {
}

DegreeDistribution::~DegreeDistribution() {
}

IdealSoliton::IdealSoliton(){
    
}

void IdealSoliton::Sample(BipartiteGraph *graph){
    
}

RobustSoliton::RobustSoliton(unsigned int max):maxDegree(0){
    ;
}

RobustSoliton::RobustSoliton(unsigned int max):maxDegree(max){
    ;
}

void RobustSoliton::Sample(BipartiteGraph *graph){
    unsigned int nvar=graph->n1;
    unsigned int ncheck=graph->n2;
    
}