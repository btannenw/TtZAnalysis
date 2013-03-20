#include "../interface/elecRhoIsoAdder.h"

namespace top{


  void elecRhoIsoAdder::checkCorrectInput(){
    if(!incorr_ && cone_!=0.3f) std::cout << "elecRhoIsoAdder: cone not properly set!! (" << cone_ << ")" << std::endl; 
    else incorr_=true;
  }

  double elecRhoIsoAdder::getRhoIso(top::NTElectron & elec){

    double chargedH  = elec.iso().chargedHadronIso();
    double neutralH  = elec.iso().neutralHadronIso();
    double photon    = elec.iso().photonIso();

    double rhoIso=(chargedH + std::max(neutralH+photon - rho_*Aeff(elec.suClu().eta()), 0.0))/elec.pt();
    return rhoIso;
  } 

  double elecRhoIsoAdder::getRhoIso(top::NTInflatedElectron & elec){

    double chargedH  = elec.iso().chargedHadronIso();
    double neutralH  = elec.iso().neutralHadronIso();
    double photon    = elec.iso().photonIso();

    double rhoIso=(chargedH + std::max(neutralH+photon - rho_*Aeff(elec.suClu().eta()), 0.0))/elec.pt();
    return rhoIso;
  } 


  void  elecRhoIsoAdder::addRhoIso(top::NTElectron & elec){
    checkCorrectInput();
    double rhoiso=getRhoIso(elec);
    elec.setRhoIso(rhoiso);
  }

  void  elecRhoIsoAdder::addRhoIso(std::vector<top::NTElectron> & electrons){
    for(std::vector<top::NTElectron>::iterator elec=electrons.begin();elec<electrons.end();++elec){
      addRhoIso(*elec);
    }
  }


  void  elecRhoIsoAdder::addRhoIso(top::NTInflatedElectron & elec){
    checkCorrectInput();
    double rhoiso=getRhoIso(elec);
    elec.setRhoIso(rhoiso);
  }

  void  elecRhoIsoAdder::addRhoIso(std::vector<top::NTInflatedElectron> & electrons){
    for(std::vector<top::NTInflatedElectron>::iterator elec=electrons.begin();elec<electrons.end();++elec){
      addRhoIso(*elec);
    }
  }

  double elecRhoIsoAdder::Aeff(double suclueta){
    double aeff=0;
    double eta=fabs(suclueta);
    if(cone_==0.3f){
      if(use2012EA_){
	if(ismc_ && 1==0){ //always false
	  //not yet provided right now the same values as for data
	  if(eta <= 1.0)                     aeff = 0.13;
	  if(1.0    <= eta && eta < 1.479)   aeff = 0.14;
	  if(1.479  <= eta && eta < 2.0)     aeff = 0.07;
	  if(2.0    <= eta && eta < 2.2)     aeff = 0.09;
	  if(2.2    <= eta && eta < 2.3)     aeff = 0.11;
	  if(2.3    <= eta && eta < 2.4)     aeff = 0.11;
	  if(2.4   <= eta)                   aeff = 0.14; 
	}
	else{ //data
	  if(eta <= 1.0)                     aeff = 0.13;
	  if(1.0    <= eta && eta < 1.479)   aeff = 0.14;
	  if(1.479  <= eta && eta < 2.0)     aeff = 0.07;
	  if(2.0    <= eta && eta < 2.2)     aeff = 0.09;
	  if(2.2    <= eta && eta < 2.3)     aeff = 0.11;
	  if(2.3    <= eta && eta < 2.4)     aeff = 0.11;
	  if(2.4   <= eta)                   aeff = 0.14; 
	}
      }
      else{ // 2011 corrections
	if(ismc_){
	  if(eta <= 1.0)                     aeff = 0.110; //1
	  if(1.0    <= eta && eta < 1.479)   aeff = 0.130; //1
	  if(1.479  <= eta && eta < 2.0)     aeff = 0.089; //1
	  if(2.0    <= eta && eta < 2.2)     aeff = 0.130; //1
	  if(2.2    <= eta && eta < 2.3)     aeff = 0.150; //2
	  if(2.3    <= eta && eta < 2.4)     aeff = 0.160; //3
	  if(2.4   <= eta)                   aeff = 0.190; //3	  
	}
	else{ //data
	  if(eta <= 1.0)                     aeff = 0.100; //2
	  if(1.0    <= eta && eta < 1.479)   aeff = 0.120; //3
	  if(1.479  <= eta && eta < 2.0)     aeff = 0.085; //2
	  if(2.0    <= eta && eta < 2.2)     aeff = 0.110; //3
	  if(2.2    <= eta && eta < 2.3)     aeff = 0.120; //4
	  if(2.3    <= eta && eta < 2.4)     aeff = 0.120; //5
	  if(2.4   <= eta)                   aeff = 0.130; //6
	}
      }
    }
    else if(cone_==0.4f){
      ///////not implemented, just for future use?
    }
    return aeff;
  }


}
