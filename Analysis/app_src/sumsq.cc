/*
 * sumsq.cc
 *
 *  Created on: Mar 19, 2014
 *      Author: kiesej
 */

#include "TtZAnalysis/Tools/interface/optParser.h"
#include <cmath>
#include <vector>
#include <iostream>

int main(int argc, char* argv[]){
    using namespace ztop;
    optParser parser(argc,argv);
    std::vector<float> nums=parser.getRest<float>();

    float sum2=0;
    for(size_t i=0;i<nums.size();i++)
        sum2+=nums.at(i)*nums.at(i);

    std::cout <<  sqrt(sum2) <<std::endl;

    return 0;
}
