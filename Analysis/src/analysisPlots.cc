/*
 * analysisPlots.cc
 *
 *  Created on: Feb 12, 2014
 *      Author: kiesej
 */

#include "../interface/analysisPlots.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"

namespace ztop{


container1DUnfold* analysisPlots::addPlot(const std::vector<float>&genbins, const std::vector<float>&recobins,
        const TString&name, const TString &xname, const TString &yname){

    bool tmp=container1DUnfold::c_makelist;
    container1DUnfold::c_makelist=true;
    container1DUnfold*newplot=new container1DUnfold(genbins,recobins,name+" step "+toTString(step_), xname, yname);
    container1DUnfold::c_makelist=tmp;
    conts_.push_back(newplot);
    return newplot;
}

std::vector<float> analysisPlots::makebins(size_t nbins,float min,float max)const{
	std::vector<float> out;
	float step=(max-min) / (float)nbins;
	float last=min;
	for(size_t i=0;i<=nbins;i++){
		out.push_back(last);
		last+=step;
	}
	return out;
}


}//ns
