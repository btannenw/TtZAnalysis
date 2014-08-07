/*
 * formatter.cc
 *
 *  Created on: Jul 28, 2014
 *      Author: kiesej
 */



#include "../interface/formatter.h"
#include "../interface/fileReader.h"

namespace ztop{
bool  formatter::debug;

void formatter::readInNameTranslateFile(const std::string & pathtofile){
	fileReader fr;
	fr.setComment("#");
	fr.setDelimiter(",");

	fr.readFile(pathtofile);

	for(size_t i=0;i<fr.nLines();i++){
		if(fr.getData(i).size() < 2) continue;
		if(debug)
			std::cout << "formatter::readInSysnamesFormatFile: reading: " << fr.getData<TString>(i,0) << " "
			<< fr.getData<TString>(i,1)  << std::endl;
		cachedNames_[fr.getData<TString>(i,0)] = fr.getData<TString>(i,1);
	}

}
 TString formatter::translateName(const TString& techname)const{
	std::map<TString,TString>::const_iterator mit=cachedNames_.find(techname);
	TString retname;
	if(mit!=cachedNames_.end())
		retname= mit->second;
	if(rootlatex_){
		retname.ReplaceAll("$","");
		retname.ReplaceAll("\\","#");
	}
	if(retname.Length()>0)
		return retname;
	if(debug)
		std::cout << "formatter::translateSysName: " << techname<< " not found "<<std::endl;
	return techname;
}


}