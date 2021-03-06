/*
 * tTreeHandler.cc
 *
 *  Created on: Apr 20, 2015
 *      Author: kiesej
 */




#include "../interface/tTreeHandler.h"
#include "../interface/tBranchHandler.h"
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"

namespace ztop{

bool  tTreeHandler::debug=false;

tTreeHandler::tTreeHandler(const TString & filename, const TString &treename):file_(0), t_(0),entry_(0),entries_(0){
	load(filename,treename);
}
tTreeHandler::tTreeHandler(): file_(0), t_(0),entry_(0),entries_(0){

}
tTreeHandler::~tTreeHandler(){
	clear();
}

void tTreeHandler::load(const TString & filename, const TString &treename){
	clear();
	std::ifstream FileTest(filename.Data());
	bool exists = (bool) FileTest;
	FileTest.close();
	if(!exists){
		std::string errstr="tTreeHandler::load: file does not exist: ";
		errstr+=filename.Data();
		throw std::runtime_error(errstr);
	}
	AutoLibraryLoader::enable();
	TFile* f=new TFile(filename,"READ");
	if(!f || f->IsZombie()){
		clear();
		throw std::runtime_error("tTreeHandler::load: file not ok.");
	}
	TTree * t = (TTree*)f->Get(treename);
	if(!t || t->IsZombie()){
		clear();
		throw std::runtime_error("tTreeHandler::load: tree not ok");
	}
	//all ok
	t_=t;
	file_=f;

	entries_=t_->GetEntries();
}
void tTreeHandler::clear(){
	if(debug)
		std::cout << "tTreeHandler::clear" << std::endl;
	for(size_t i=0;i<assobranches_.size();i++){
		assobranches_.at(i)->removeTree(this);
	}
	if(debug)
		std::cout << "tTreeHandler::clear: removed tree from branches" << std::endl;
	assobranches_.clear(); //just remove asso
	if(debug)
		std::cout << "tTreeHandler::clear: removed branches from tree" << std::endl;
	if(t_)
		delete t_;
	t_=0;
	if(debug)
		std::cout << "tTreeHandler::clear: deleted tree" << std::endl;

	if(file_){
		if(file_->IsOpen())
			file_->Close();
		if(debug)
			std::cout << "tTreeHandler::clear: closed TFile" << std::endl;
		delete file_;
		if(debug)
			std::cout << "tTreeHandler::clear: deleted TFile" << std::endl;
	}
	file_=0;
}
void tTreeHandler::associate( tBranchHandlerBase*tb){
	assobranches_.push_back(tb);
}
void tTreeHandler::removeAsso( tBranchHandlerBase*tb){
	if(tb){
		for(size_t i=0;i<assobranches_.size();i++){
			if(assobranches_.at(i) == tb){
				std::vector<tBranchHandlerBase*>::iterator it=assobranches_.begin()+i;
				assobranches_.erase(it);
			}
		}
	}
}

void tTreeHandler::setEntry(const Long64_t& in){

	entry_ = in;
	for(size_t i=0;i<assobranches_.size();i++)
		assobranches_.at(i)->newEntry();
	tree()->LoadTree(entry_);
}
void tTreeHandler::setPreCache(){
	struct stat filestatus;
	stat(file_->GetPath(), &filestatus );
	Long64_t cache=filestatus.st_size/10;
	tree()->SetCacheSize(filestatus.st_size/10);
	if(cache > 20e6)
		tree()->SetCacheSize(20e6);
	else
		tree()->SetCacheSize(cache/10);
	tree()->SetCacheLearnEntries(200);

}

void tTreeHandler::printStats()const{
	if(file_)
		printf("Reading %lld bytes in %d transactions\n",file_->GetBytesRead(),  file_->GetReadCalls());
}

}
