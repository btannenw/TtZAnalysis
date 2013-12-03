/*
 * eventLoop.h
 *
 *  Created on: Jul 17, 2013
 *      Author: kiesej
 */

#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_
#include "../interface/MainAnalyzer.h"
#include "../interface/AnalysisUtils.h"
#include "../interface/NTFullEvent.h"
#include "../interface/ttbarControlPlots.h"
#include "../interface/ZControlPlots.h"
#include "TtZAnalysis/plugins/leptonSelector2.h"

#include "TtZAnalysis/DataFormats/interface/NTLepton.h"
#include "TtZAnalysis/DataFormats/interface/NTMuon.h"
#include "TtZAnalysis/DataFormats/interface/NTElectron.h"
#include "TtZAnalysis/DataFormats/interface/NTJet.h"
#include "TtZAnalysis/DataFormats/interface/NTMet.h"
#include "TtZAnalysis/DataFormats/interface/NTEvent.h"
#include "TtZAnalysis/DataFormats/interface/NTIsolation.h"
#include "TtZAnalysis/DataFormats/interface/NTTrack.h"
#include "TtZAnalysis/DataFormats/interface/NTSuClu.h"
#include "TtZAnalysis/DataFormats/interface/NTTrigger.h"
#include "TtZAnalysis/DataFormats/interface/NTTriggerObject.h"


#include "TtZAnalysis/DataFormats/interface/NTVertex.h"

#include "TtZAnalysis/DataFormats/interface/NTGenLepton.h"
#include "TtZAnalysis/DataFormats/interface/NTGenParticle.h"
#include "TtZAnalysis/DataFormats/interface/NTGenJet.h"



void  MainAnalyzer::analyze(TString inputfile, TString legendname, int color, double norm,size_t legord, size_t anaid){

	bool doLargeAcceptance=false;
	/*means right now:
	 *
	 * leps: 20/10
	 * jets: 30/20
	 *
	 */

	using namespace std;
	using namespace ztop;


	//some mode options
	//if(mode_=="xsec"); //standard
	bool mode_samesign=false;
	if(mode_.Contains("samesign")){
		mode_samesign=true;
		std::cout << "entering same sign mode" <<std::endl;
	}
	size_t mintightmuons=0;
	if(mode_.Contains("tightmuons")){
		mintightmuons=1;
		if(b_mumu_)
			mintightmuons=2;
		std::cout << "entering tight muon mode" <<std::endl;
	}
	bool mode_invertiso=false;
	if(mode_.Contains("invertiso")){
		mode_invertiso=true;
		std::cout << "entering invert iso mode" <<std::endl;
	}

	bool issignal=issignal_.at(anaid);

	containerStackVector::debug=true;

	bool isMC=true;
	if(legendname==dataname_) isMC=false;

	//per sample configuration

	getTriggerSF()->setRangeCheck(false);
	getElecSF()->setRangeCheck(false);
	getMuonSF()->setRangeCheck(false);
	getElecSF()->setIsMC(isMC);
	getMuonSF()->setIsMC(isMC);
	getTriggerSF()->setIsMC(isMC);

	//some global checks
	getElecEnergySF()->setRangeCheck(false);
	getMuonEnergySF()->setRangeCheck(false);
	getElecEnergySF()->setIsMC(isMC);
	getMuonEnergySF()->setIsMC(isMC);

	//check if file exists
	if(testmode_)
		std::cout << "testmode("<< anaid << "): check input file" << std::endl;

	TFile *f;
	if(!fileExists((datasetdirectory_+inputfile).Data())){
		std::cout << datasetdirectory_+inputfile << " not found!!" << std::endl;
		p_finished.get(anaid)->pwrite(-1);
		return;
	}
	else{
		f=TFile::Open(datasetdirectory_+inputfile);
	}


	//define containers here

	//   define some norm containers

	if(testmode_)
		std::cout << "testmode("<< anaid << "): defining bins" << std::endl;

	vector<float> onebin;
	onebin << 0.5 << 1.5;
	container1D generated(onebin, "generated events", "gen", "N_{gen}");
	container1D generated2(onebin, "generated filtered events", "gen", "N_{gen}");



	container1D::c_makelist=false; //switch off automatic listing

	if(testmode_)
		std::cout << "testmode("<< anaid << "): preparing container1DUnfolds" << std::endl;

	//////////////UNFOLDING DISTRIBUTIONS/////////////
	container1DUnfold::c_makelist=true;

	//some other distributions
	///MLB

	vector<float> genmlb_bins;
	genmlb_bins  << 0 << 50 << 90 << 120 << 140 << 160 << 230  << 350;
	vector<float> ivangen_mlbbins;
	ivangen_mlbbins << 0 << 70 << 116 << 150 <<400;
	vector<float> mlb_bins=ztop::subdivide<float>(genmlb_bins,5);
	vector<float> ivan_mlbbins=subdivide<float>(ivangen_mlbbins,5);

	container1DUnfold unf_mlb8(genmlb_bins,mlb_bins,"m_lb unfold step 8","M_{lb} [GeV]", "N/GeV");

	container1DUnfold unf_ivanmlb8(ivangen_mlbbins,ivan_mlbbins,"m_lb ivansbins unfold step 8","M_{lb} [GeV]", "N/GeV");
	container1DUnfold unf_mlbbbb8(genmlb_bins,mlb_bins,"m_lb unfold BBB step 8","M_{lb} [GeV]", "N/GeV");
	unf_mlbbbb8.setBinByBin(true);

	container1DUnfold unf_mlb9(genmlb_bins,mlb_bins,"m_lb unfold step 9","M_{lb} [GeV]", "N/GeV");

	genmlb_bins=ztop::subdivide<float>(genmlb_bins,5);
	mlb_bins=ztop::subdivide<float>(mlb_bins,5);
	container1DUnfold unf_mlbfb8(genmlb_bins,mlb_bins,"m_lb unfold finebinned step 8","M_{lb} [GeV]", "N/GeV");


	///lepton quantities

	vector<float> genlpt_bins; genlpt_bins  << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 100 << 130 << 160 << 250;
	vector<float> recolpt_bins= subdivide<float>(genlpt_bins,10);
	container1DUnfold unf_ptl(genlpt_bins,recolpt_bins,"lep_plus pt unfold step 8","p_{T}^{l} [GeV]", "N/GeV");

	vector<float> genllpt_bins; genllpt_bins << 40  << 60  << 70 << 80  << 90 << 100 << 130 << 160 << 250<<350 <<400 << 500;
	vector<float> recollpt_bins= subdivide<float>(genllpt_bins,10);
	container1DUnfold unf_ptll(genllpt_bins,recollpt_bins,"sum lep pt unfold step 8","p_{T}^{l1}+p_{T}^{l2} [GeV]", "N/GeV");

	vector<float> genllE_bins; genllE_bins << 40 << 50 << 60 << 80 << 90 << 100 << 110
	<< 120 << 130 << 140 << 150 << 160 << 180 << 200 << 210 << 220 << 230 << 240 <<250 << 350 << 450 << 600;
	vector<float> recollE_bins= subdivide<float>(genllE_bins,10);
	container1DUnfold unf_Ell(genllE_bins,recollE_bins,"sum lep E unfold step 8","E^{l1}+E^{l2} [GeV]", "N/GeV");

	vector<float> gendilpt_bins; gendilpt_bins << 0 << 10 << 20 << 30 << 40 << 50 << 60 << 80 << 100 << 110 << 120 << 130 << 150 << 250;
	vector<float> recodilpt_bins= subdivide<float>(gendilpt_bins,10);
	container1DUnfold unf_dilpt(gendilpt_bins,recodilpt_bins,"dilepton pt unfold step 8","p_{T}^{ll} [GeV]", "N/GeV");

	vector<float> genmll_bins; genmll_bins  << 20 << 30 << 40 << 50 << 60 << 70 << 80 << 90 << 100 << 110 << 120 << 130 << 140 << 150 <<  160 << 200 << 250 << 350;
	vector<float> recomll_bins= subdivide<float>(genmll_bins,10);
	container1DUnfold unf_mll(genmll_bins,recomll_bins,"dilepton mass unfold step 8","m_{ll} [GeV]", "N/GeV");




	vector<float> inclbins; inclbins << 0.5 << 1.5; //vis PS, fullPS

	container1DUnfold unf_tot8(inclbins,inclbins,"total step 8","bin","N_{evt}");
	unf_tot8.setBinByBin(true); //independent bins

	if(b_emu_) container1DUnfold::c_makelist=false;

	vector<float> Zptgen_bins;
	Zptgen_bins << 0 << 1 << 5 << 10 << 15 << 20 << 30 << 40 << 50 << 70 << 100 << 150 << 200 << 300;
	vector<float> Zptreco_bins=subdivide(Zptgen_bins,10);


	container1DUnfold unf_Zpt(Zptgen_bins, Zptreco_bins, "Z pt step 20", "P_{T}^{Z} [GeV]","N_{evt}");

	//setting the right normalisation for MC and the histos for inclusive cross section determination
	container1DUnfold::c_makelist=false;
	container1DUnfold::setAllListedMC(isMC);
	container1DUnfold::setAllListedLumi((float)lumi_);

	double genentries=0;
	if(isMC){
		if(testmode_)
			std::cout << "testmode("<< anaid << "): getting genTree for normalisation" << std::endl;

		TTree * tnorm = (TTree*) f->Get("preCutPUInfo/preCutPUInfo");
		genentries=tnorm->GetEntries();

		delete tnorm;
		norm = lumi_ * norm / genentries;
		for(size_t i=1;i<=generated.getNBins();i++){
			generated.setBinContent(i, (float)genentries);
			generated.setBinStat(i,sqrt(genentries));
		}
		double fgen=0;
		TTree * tfgen = (TTree*) f->Get("postCutPUInfo/PUTreePostCut");
		if(tfgen){
			fgen=tfgen->GetEntries();
			for(size_t i=1;i<=generated2.getNBins();i++){
				generated2.setBinContent(i, (float)fgen);
				generated2.setBinStat(i, sqrt(fgen));
			}
		}
		else{
			for(size_t i=1;i<=generated.getNBins();i++){
				generated2.setBinContent(i, (float)genentries);
				generated2.setBinStat(i, sqrt(genentries));
			}
		}

	}
	else{//not mc
		for(size_t i=1;i<=generated.getNBins();i++){
			generated.setBinContent(i, 0);
			generated.setBinStat(i, 0);
		}
		for(size_t i=1;i<=generated2.getNBins();i++){
			generated2.setBinContent(i, 0);
			generated2.setBinStat(i, 0);
		}
		norm=1;
	}

	/////////////////////////// configure scalefactors ////
	if(testmode_)
		std::cout << "testmode("<< anaid << "):  configuring scalefactors" << std::endl;



	//init b-tag scale factor utility
	if(testmode_)
		std::cout << "testmode("<< anaid << "): preparing btag SF" << std::endl;

	if(getBTagSF()->setSampleName(toString(inputfile)) < 0){
		p_finished.get(anaid)->pwrite(-2);
		return;
	}

	//cout << "running on: " << datasetdirectory_ << inputfile << "    legend: " << legendname << "\nxsec: " << oldnorm << ", genEvents: " << genentries <<endl;
	// get main analysis tree

	/////////open main tree and prepare collections


	TTree * t = (TTree*) f->Get("PFTree/PFTree");

	TBranch * b_TriggerBools=0;
	std::vector<bool> * p_TriggerBools=0;
	t->SetBranchAddress("TriggerBools",&p_TriggerBools, &b_TriggerBools);

	//0 Ele
	//1,2 Mu

	TBranch * b_Electrons=0;
	vector<NTElectron> * pElectrons = 0;
	// t->SetBranchAddress("NTPFElectrons",&pElectrons,&b_Electrons);
	t->SetBranchAddress("NTElectrons",&pElectrons,&b_Electrons);

	TBranch * b_Muons=0;
	vector<NTMuon> * pMuons = 0;
	t->SetBranchAddress("NTMuons",&pMuons, &b_Muons);

	TBranch * b_Jets=0;         // ##TRAP##
	vector<NTJet> * pJets=0;
	t->SetBranchAddress("NTJets",&pJets, &b_Jets);

	TBranch * b_Met=0;
	NTMet * pMet = 0;
	t->SetBranchAddress("NTMet",&pMet,&b_Met);

	TBranch * b_Event=0;
	NTEvent * pEvent = 0;
	t->SetBranchAddress("NTEvent",&pEvent,&b_Event);

	/// generator branches should exist everywhere but empty for non signal processes

	TBranch * b_GenTops=0;
	vector<NTGenParticle> * pGenTops=0;
	t->SetBranchAddress("NTGenTops",&pGenTops,&b_GenTops);
	// TBranch * b_GenWs=0;
	TBranch * b_GenZs=0;
	vector<NTGenParticle> * pGenZs=0;
	t->SetBranchAddress("NTGenZs",&pGenZs,&b_GenZs);
	// TBranch * b_GenBs=0;
	TBranch * b_GenBHadrons=0;
	vector<NTGenParticle> * pGenBHadrons=0;
	t->SetBranchAddress("NTGenBHadrons",&pGenBHadrons,&b_GenBHadrons);
	TBranch * b_GenLeptons3=0;
	vector<NTGenParticle> *pGenLeptons3 =0;
	t->SetBranchAddress("NTGenLeptons3",&pGenLeptons3,&b_GenLeptons3);
	TBranch * b_GenLeptons1=0;
	vector<NTGenParticle> *pGenLeptons1 =0;
	t->SetBranchAddress("NTGenLeptons1",&pGenLeptons1,&b_GenLeptons1);

	TBranch * b_GenJets=0;
	vector<NTGenJet> * pGenJets=0;
	t->SetBranchAddress("NTGenJets",&pGenJets,&b_GenJets);

	TBranch * b_GenNeutrinos=0;
	vector<NTGenParticle> * pGenNeutrinos=0;
	t->SetBranchAddress("NTGenNeutrinos",&pGenNeutrinos,&b_GenNeutrinos);


	/*
	 * ONLY because of a bug in current trees
	 */
	elecRhoIsoAdder elecrhoisoadd(isMC);
	elecrhoisoadd.setUse2012EA(!is7TeV_);


	double sel_step[]={0,0,0,0,0,0,0,0,0};
	float count_samesign=0;
	///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
	///////////////////////////////////////// start main loop /////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////// /////////////////////////////////////////////////////////

	if(testmode_)
		std::cout << "testmode("<< anaid << "): starting main loop" << std::endl;



	container1D::c_makelist =true;
	//define event
	NTFullEvent evt;
	ttbarControlPlots plots;//for the Z part just make another class (or add a boolian)..
	ZControlPlots zplots;
	plots.setEvent(evt);
	zplots.setEvent(evt);

	float tpuweight=0;
	evt.puweight=&tpuweight;
	//get the plots in the right order. not necessary but nicer
	for(size_t i=0;i<10;i++){ //steps
		plots.makeControlPlots(i);
		zplots.makeControlPlots(i);
	}
	//switch on listing
	/*vector<float> testbins;
	for(float i=0;i<200;i++) testbins << i;
	vector<container1D*> testconts;
	size_t ntestconts=10000;
	for(size_t i=0;i<ntestconts;i++)
		testconts.push_back(new container1D(testbins));
	 */
	Long64_t nEntries=t->GetEntries();
	if(norm==0) nEntries=0; //skip for norm0
	if(testmode_) nEntries*=0.08;
	for(Long64_t entry=0;entry<nEntries;entry++){
		//if(showstatusbar_) displayStatusBar(entry,nEntries);


		size_t step=0;
		evt.reset();

		container1DUnfold::flushAllListed();

		if((entry +1)* 100 % nEntries <100){
			int status=(entry+1) * 100 / nEntries;
			p_status.get(anaid)->pwrite(status);
			if(singlefile_)
				std::cout <<"single run " << legendname <<"  "<< status << "%" << std::endl;
		}

		b_Event->GetEntry(entry);
		float puweight=1;
		if (isMC) puweight = getPUReweighter()->getPUweight(pEvent->truePU());
		if(testmode_ && entry==0)
			std::cout << "testmode("<< anaid << "): got first event entry" << std::endl;

		evt.puweight=&puweight;



		bool visPS=true;
		///gen stuff wo cuts!
		if(isMC){
			/* define all generator variables here as 0
			 * if no gen info available (background samples e.g.)
			 * they are just filled with zeros
			 */

			if(testmode_ && entry==0)
				std::cout << "testmode("<< anaid << "): got first MC gen entry" << std::endl;

			double mlbgen=-1;
			NTGenParticle * genlep_plus=0;NTGenParticle *genlep_minus=0;

			b_GenLeptons3->GetEntry(entry);
			if(pGenLeptons3->size()>1){ //gen info there
				visPS=false;
				if(testmode_ && entry==0){
					std::cout << "testmode("<< anaid << "): entered signal genInfo part" << std::endl;
				}
				//bool isDilepton=false;
				b_GenBHadrons->GetEntry(entry);

				b_GenJets->GetEntry(entry);
				b_GenLeptons1->GetEntry(entry);


				if(pGenLeptons3->size()>1 && testmode_ && false){
					std::cout
					<< pGenLeptons3->at(0).pt() << "," << pGenLeptons3->at(0).eta() << " \t"
					<< pGenLeptons3->at(1).pt() << " " << pGenLeptons3->at(1).eta() << " \n"

					<<std::endl;
					p_finished.get(anaid)->pwrite(34);
					return;
				}


				//recreate mother daughter relations?!
				b_GenBHadrons->GetEntry(entry);

				b_GenJets->GetEntry(entry);
				b_GenLeptons1->GetEntry(entry);
				//visible phase space cuts
				vector<NTGenParticle *> visGenLeptons1,visGenLeptons3;

				for(size_t i=0;i<pGenLeptons1->size();i++){
					NTGenParticle * lep=&(pGenLeptons1->at(i));
					if(lep->pt()>20 && fabs(lep->eta())<2.5)
						visGenLeptons1.push_back(lep);
				}
				for(size_t i=0;i<pGenLeptons3->size();i++){
					NTGenParticle * lep=&(pGenLeptons3->at(i));
					if(lep->pt()>20 && fabs(lep->eta())<2.4)
						visGenLeptons3.push_back(lep);
				}

				vector<NTGenJet *> visGenJets;
				for(size_t i=0;i<pGenJets->size();i++){
					NTGenJet * genjet=&(pGenJets->at(i));
					if(genjet->pt()>30 && fabs(genjet->eta())<2.5)
						visGenJets.push_back(genjet);
				}
				///make vector of pointers (NOT constant!!)

				if(visGenLeptons1.size()>2
						&& visGenJets.size()>2)
					visPS=true;

				std::vector<int> bhadids;
				for(size_t i=0;i<pGenBHadrons->size();i++){
					NTGenParticle * bhad=&pGenBHadrons->at(i);

					bhadids<<bhad->genId();

				}
				PolarLorentzVector p4genbjet;
				vector<NTGenJet *> genbjets;;
				for(size_t i=0;i<pGenJets->size();i++){
					NTGenJet * genjet=&pGenJets->at(i);
					if(genjet->motherIts().size()>0){
						int motherid=genjet->motherIts().at(0);
						if(-1<isIn(motherid,bhadids)){
							genbjets << genjet;
						}
					}
				}
				vector<NTGenJet *> visgenbjets;;
				for(size_t i=0;i<genbjets.size();i++){
					NTGenJet * bjet=genbjets.at(i);
					if(bjet->pt()<30) continue;
					if(fabs(bjet->eta()) >2.5) continue;
					visgenbjets << bjet;
				}


				if(visgenbjets.size()>0)
					p4genbjet=visgenbjets.at(0)->p4();

				if(visGenLeptons1.size() > 1){ ///
					PolarLorentzVector p4leadinglep=visGenLeptons1.at(0)->p4();
					mlbgen=(p4genbjet+p4leadinglep).M();

					//PolarLorentzVector p4dil=pGenLeptons1->at(0).p4() + pGenLeptons1->at(1).p4();

				}
				if(b_GenZs){

					b_GenZs->GetEntry(entry);

					if(pGenZs && pGenZs->size()>0){
						unf_Zpt.fillGen(pGenZs->at(0).pt(),puweight);
					}
				}
				//recreateRelations(mothers, daughters) --serach for genid and motherit, daugherits

				//leptons

				/*
					container1DUnfold unf_ptl(genlpt_bins,recolpt_bins,"lep+ pt unfold step 8","p_{T}^{l} [GeV]", "N/GeV");

					container1DUnfold unf_ptll(genllpt_bins,recollpt_bins,"sum lep pt unfold step 8","p_{T}^{l1}+p_{T}^{l2} [GeV]", "N/GeV");

					container1DUnfold unf_Ell(genllE_bins,recollE_bins,"sum lep E unfold step 8","E^{l1}+E^{l2} [GeV]", "N/GeV");

					container1DUnfold unf_dilpt(gendilpt_bins,recodilpt_bins,"dilepton pt unfold step 8","p_{T}^{ll} [GeV]", "N/GeV");

					container1DUnfold unf_mll(genmll_bins,recomll_bins,"dilepton mass unfold step 8","m_{ll} [GeV]", "N/GeV");
				 */

				///THIS IS A WORKAROUND!!!
				for(size_t i=0;i<visGenLeptons1.size();i++){
					switch(i){
					case 0:  genlep_plus=visGenLeptons1.at(i);break;
					case 1:  genlep_minus=visGenLeptons1.at(i);break;
					}
				}

			}
			/*
			 * fill gen info here
			 */
			//float temppuweight=puweight;
			//puweight=1;
			if(mlbgen>=0){
				unf_ivanmlb8.fillGen(mlbgen,puweight);
				unf_mlb8.fillGen(mlbgen,puweight);
				unf_mlbfb8.fillGen(mlbgen,puweight);
				unf_mlbbbb8.fillGen(mlbgen,puweight);
				unf_mlb9.fillGen(mlbgen,puweight);
			}

			if(genlep_plus && genlep_minus){
				unf_ptl.fillGen(genlep_plus->pt(),puweight);
				unf_ptl.fillGen(genlep_minus->pt(),puweight);

				unf_ptll.fillGen(genlep_plus->pt()+genlep_minus->pt(),puweight);
				unf_Ell.fillGen(genlep_plus->E()+genlep_minus->E(),puweight);
				unf_dilpt.fillGen((genlep_plus->p4()+genlep_minus->p4()).Pt(),puweight);
				unf_mll.fillGen((genlep_plus->p4()+genlep_minus->p4()).M(),puweight);
			}

			if(visPS) unf_tot8.fillGen(0,puweight);
			unf_tot8.fillGen(1,puweight); //fullPS,
			//puweight=temppuweight;

		} /// isMC ends




		/*
		 *  Trigger
		 */
		b_TriggerBools->GetEntry(entry);
		if(testmode_ && entry==0)
			std::cout << "testmode("<< anaid << "): got trigger boolians" << std::endl;
		if(!checkTrigger(p_TriggerBools,pEvent, isMC,anaid)) continue;


		/////////////////////AFTER TRIGGER//////////////////
		////////////////////////////////////////////////////
		////////////////////MAKE COLLECTIONS////////////////
		////////////////////////////////////////////////////
		////////////////////LINK TO EVENT///////////////////

		evt.event=pEvent;

		/*
		 * Muons
		 */
		b_Muons->GetEntry(entry);


		vector<NTLepton *> allleps;

		vector<NTMuon*> kinmuons,idmuons,isomuons,tightmuons,loosemuons;
		evt.kinmuons=&kinmuons;
		evt.idmuons=&idmuons;
		evt.isomuons=&isomuons;

		//vector<NTMuon*> kinmuons,idmuons,isomuons,tightmuons,loosemuons;
		bool gotfirst=false;
		for(size_t i=0;i<pMuons->size();i++){
			NTMuon* muon = & pMuons->at(i);
			muon->setP4(muon->p4() * getMuonEnergySF()->getScalefactor(muon->eta()));
			allleps << muon;
			if(!gotfirst && muon->pt() < 20)       continue;
			if(doLargeAcceptance) gotfirst=true;
			if(muon->pt() < 10) continue;
			if(fabs(muon->eta())>2.4) continue;
			kinmuons << &(pMuons->at(i));
			///select id muons
			if(!(muon->isGlobal() || muon->isTracker()) ) continue;
			//try with tight muons

			if(mintightmuons && muon->isGlobal()
					&& muon->normChi2()<10.
					&& muon->muonHits()>0
					&& muon->getMember(0) >1  //not in trees
					&& fabs(muon->d0V())<0.2
					&& fabs(muon->dzV())<0.2
					&& muon->pixHits()>0
					&& muon->trkHits()>5){
				tightmuons <<  &(pMuons->at(i));
				continue; //no double counting
			}
			//usetight=false;

			///end tight
			loosemuons <<  &(pMuons->at(i));
		}

		idmuons << tightmuons << loosemuons; //merge collections

		for(size_t i=0;i<idmuons.size();i++){
			NTMuon * muon =  idmuons.at(i);
			if(!mode_invertiso && muon->isoVal() > 0.15) continue;
			if(mode_invertiso && muon->isoVal() < 0.15) continue;
			isomuons <<  muon;

		}

		/*
		 * Electrons
		 */

		b_Electrons->GetEntry(entry);

		vector<NTElectron *> kinelectrons,idelectrons,isoelectrons;
		evt.kinelectrons=&kinelectrons;
		evt.idelectrons=&idelectrons;
		evt.isoelectrons=&isoelectrons;
		gotfirst=false;
		for(size_t i=0;i<pElectrons->size();i++){
			NTElectron * elec=&(pElectrons->at(i));
			double ensf=getElecEnergySF()->getScalefactor(elec->eta());
			elec->setECalP4(elec->ECalP4() * ensf);
			elec->setP4(elec->ECalP4() * ensf); //both the same now!!
			allleps << elec;
			if(!gotfirst && elec->ECalP4().Pt() < 20)  continue;
			if(doLargeAcceptance) gotfirst=true;
			if( elec->ECalP4().Pt() < 10) continue;
			if(fabs(elec->eta()) > 2.4) continue;      ///common muon/elec phasespace
			//if(!noOverlap(&(pElectrons->at(i)), kinmuons, 0.1)) continue;   ////!!!CHANGE
			kinelectrons  << elec;

			///select id electrons
			if(fabs(elec->d0V()) >0.04 ) continue;
			if(!(elec->isNotConv()) ) continue;
			if(elec->mvaId() < 0.5) continue;
			if(elec->mHits() > 0) continue;

			/////////only temporarily////!! rho iso stuff
			//NTSuClu suclu;
			//LorentzVector ecalp4;
			//ecalp4=pElectrons->at(i).ECalP4();
			//suclu.setP4(ecalp4);
			//elec->setSuClu(suclu);
			//elecrhoisoadd.addRhoIso(*elec);

			idelectrons <<  elec;

			//select iso electrons
			if(!mode_invertiso && elec->rhoIso()>0.15) continue;
			if(mode_invertiso && elec->rhoIso()<0.15) continue;
			isoelectrons <<  elec;
		}

		/*
		 * Make some plots of initial lepton distributions
		 */

		std::sort(allleps.begin(),allleps.end(), comparePt<ztop::NTLepton*>);
		evt.allleptons=&allleps;


		/*
		 * Step 0 kin electrons and muons
		 */

		if(b_mumu_ && kinmuons.size()<2)
			continue;
		if(b_ee_ && kinelectrons.size() <2)
			continue;
		if(b_emu_ && kinelectrons.size() + kinmuons.size() < 2)
			continue;

		/*
		 * Jets
		 */
		/*	for(size_t i=0;i<ntestconts;i++)
				testconts.at(i)->fill(1); */


		sel_step[0]+=puweight;
		plots.makeControlPlots(step);
		zplots.makeControlPlots(step);
		//////////two ID leptons STEP 1///////////////////////////////
		step++;


		if(b_ee_ && idelectrons.size() < 2) continue;
		if(b_mumu_ && (idmuons.size() < 2 || tightmuons.size() < mintightmuons)) continue;
		if(b_emu_ && (idmuons.size() + idelectrons.size() < 2 || tightmuons.size() < mintightmuons)) continue;

		sel_step[1]+=puweight;
		plots.makeControlPlots(step);
		zplots.makeControlPlots(step);



		//////// require two iso leptons  STEP 2  //////
		step++;

		if(b_ee_ && isoelectrons.size() < 2) continue;
		if(b_mumu_ && isomuons.size() < 2 ) continue;
		if(b_emu_ && isomuons.size() + isoelectrons.size() < 2) continue;

		//make pair
		pair<vector<NTElectron*>, vector<NTMuon*> > oppopair,sspair;
		oppopair = ztop::getOppoQHighestPtPair(isoelectrons, isomuons);
		sspair = ztop::getOppoQHighestPtPair(isoelectrons, isomuons,-1);

		pair<vector<NTElectron*>, vector<NTMuon*> > *leppair=&oppopair;
		if(mode_samesign)
			leppair=&sspair;

		//fast count_samesign counting
		if(b_ee_ && sspair.first.size() >1)
			count_samesign+=puweight;
		else if(b_mumu_&&sspair.second.size() > 1)
			count_samesign+=puweight;
		else if(b_emu_ && sspair.first.size() > 0 && sspair.second.size() > 0)
			count_samesign+=puweight;

		float mll=0;
		LorentzVector dilp4;

		NTLepton * firstlep=0,*seclep=0, *leadingptlep=0, *secleadingptlep=0;
		double lepweight=1;
		if(b_ee_){
			if(leppair->first.size() <2) continue;
			dilp4=leppair->first[0]->p4() + leppair->first[1]->p4();
			mll=dilp4.M();
			firstlep=leppair->first[0];
			seclep=leppair->first[1];
			lepweight*=getElecSF()->getScalefactor(fabs(firstlep->eta()),firstlep->pt());
			lepweight*=getElecSF()->getScalefactor(fabs(seclep->eta()),seclep->pt());
			lepweight*=getTriggerSF()->getScalefactor(fabs(firstlep->eta()),fabs(seclep->eta()));
		}
		else if(b_mumu_){
			if(leppair->second.size() < 2) continue;
			dilp4=leppair->second[0]->p4() + leppair->second[1]->p4();
			mll=dilp4.M();
			firstlep=leppair->second[0];
			seclep=leppair->second[1];
			lepweight*=getMuonSF()->getScalefactor(fabs(firstlep->eta()),firstlep->pt());
			lepweight*=getMuonSF()->getScalefactor(fabs(seclep->eta()),seclep->pt());
			lepweight*=getTriggerSF()->getScalefactor(fabs(firstlep->eta()),fabs(seclep->eta()));
		}
		else if(b_emu_){
			if(leppair->first.size() < 1 || leppair->second.size() < 1) continue;
			dilp4=leppair->first[0]->p4() + leppair->second[0]->p4();
			mll=dilp4.M();
			firstlep=leppair->first[0];
			seclep=leppair->second[0];
			lepweight*=getElecSF()->getScalefactor(fabs(firstlep->eta()),firstlep->pt());
			lepweight*=getMuonSF()->getScalefactor(fabs(seclep->eta()),seclep->pt());
			lepweight*=getTriggerSF()->getScalefactor(fabs(firstlep->eta()),fabs(seclep->eta()));
		}
		//channel defined
		if(firstlep->pt() > seclep->pt()){
			leadingptlep=firstlep;
			secleadingptlep=seclep;
		}
		else{
			leadingptlep=seclep;
			secleadingptlep=firstlep;
		}
		//just to avoid warnings
		evt.leadinglep=leadingptlep;
		evt.secleadinglep=secleadingptlep;
		evt.mll=&mll;

		puweight*=lepweight;

		if(isMC && fabs(puweight) > 99999){
			p_finished.get(anaid)->pwrite(-88);
			return;
		}



		sel_step[2]+=puweight;
		plots.makeControlPlots(step);
		zplots.makeControlPlots(step);
		///////// 20 GeV cut /// STEP 3 ///////////////////
		step++;

		if(mll < 20)
			continue;

		// create jec jets for met and ID jets
		// create ID Jets and correct JER
		b_Jets->GetEntry(entry);

		vector<NTJet *> treejets,idjets,medjets,hardjets;
		evt.idjets=&idjets;
		evt.medjets=&medjets;
		evt.hardjets=&hardjets;
		for(size_t i=0;i<pJets->size();i++){
			treejets << &(pJets->at(i));
		}

		double dpx=0;
		double dpy=0;
		gotfirst=false;
		for(size_t i=0;i<treejets.size();i++){ //ALSO THE RESOLUTION AFFECTS MET. HERE INTENDED!!! GOOD?
			PolarLorentzVector oldp4=treejets.at(i)->p4();
			if(isMC){// && !is7TeV_){
				bool useJetForMet=false;
				if(treejets.at(i)->emEnergyFraction() < 0.9 && treejets.at(i)->pt() > 10)
					useJetForMet=true; //dont even do something

				getJECUncertainties()->applyToJet(treejets.at(i));
				getJERAdjuster()->correctJet(treejets.at(i));
				//corrected
				if(useJetForMet){
					dpx += oldp4.Px() - treejets.at(i)->p4().Px();
					dpy += oldp4.Py() - treejets.at(i)->p4().Py();
				}
			}
			if(!(treejets.at(i)->id())) continue;
			if(!noOverlap(treejets.at(i), isomuons, 0.5)) continue;
			if(!noOverlap(treejets.at(i), isoelectrons, 0.5)) continue;
			if(fabs(treejets.at(i)->eta())>2.5) continue;
			if(treejets.at(i)->pt() < 10) continue;
			idjets << (treejets.at(i));

			if(treejets.at(i)->pt() < 10) continue;
			medjets << treejets.at(i);
			if(treejets.at(i)->pt() < 30) continue;
			hardjets << treejets.at(i);
		}
		//jets
		//		std::sort
		std::sort(idjets.begin(),idjets.end(), comparePt<ztop::NTJet*>);
		std::sort(medjets.begin(),medjets.end(), comparePt<ztop::NTJet*>);
		std::sort(hardjets.begin(),hardjets.end(), comparePt<ztop::NTJet*>);



		b_Met->GetEntry(entry);

		NTMet adjustedmet = *pMet;
		evt.simplemet=pMet;
		evt.adjustedmet=&adjustedmet;
		double nmpx=pMet->p4().Px() + dpx;
		double nmpy=pMet->p4().Py() + dpy;
		adjustedmet.setP4(LorentzVector(nmpx,nmpy,0,sqrt(nmpx*nmpx+nmpy*nmpy)));

		///////////////combined variables////////////

		float dphillj=0,dphilljj=0,detallj=0,detalljj=0;
		float pi=M_PI;
		bool midphi=false;

		if(hardjets.size() >0){
			dphillj=fabs( (leadingptlep->p4()+secleadingptlep->p4()).Phi() - hardjets.at(0)->p4().Phi() );
			dphillj=pi-fabs(dphillj-pi);
			evt.dphillj=&dphillj;
			evt.midphi=&midphi;
			detallj=(leadingptlep->p4()+secleadingptlep->p4()).Eta() - hardjets.at(0)->p4().Eta();
			evt.detallj=&detallj;
		}

		if(dphillj > 2.64)
			midphi=true;

		if(hardjets.size() >1){
			dphilljj=fabs( (leadingptlep->p4()+secleadingptlep->p4()).Phi() - (hardjets.at(0)->p4()+hardjets.at(1)->p4()).Phi() );
			dphilljj=pi-fabs(dphilljj-pi);
			evt.dphilljj=&dphilljj;
			detalljj=(leadingptlep->p4()+secleadingptlep->p4()).Eta() - (hardjets.at(0)->p4()+hardjets.at(1)->p4()).Eta();
			evt.detalljj=&detalljj;
		}
		float topdiscr=0;
		evt.topdiscr=&topdiscr;

		float ptllj=leadingptlep->pt()+secleadingptlep->pt();
		evt.ptllj=&ptllj;
		if(hardjets.size() >0)
			ptllj+=hardjets.at(0)->pt();





		vector<NTJet*> hardbjets;
		evt.hardbjets=&hardbjets;
		vector<NTJet*> medbjets;
		evt.medbjets=&medbjets;
		for(size_t i=0;i<hardjets.size();i++){
			getBTagSF()->fillEff(hardjets.at(i), puweight);
			//  cout << hardjets.at(i)->genPartonFlavour() << endl;
			if(hardjets.at(i)->btag() < 0.244) continue;
			hardbjets.push_back(hardjets.at(i));
		}

		for(size_t i=0;i<medjets.size();i++){
			getBTagSF()->fillEff(medjets.at(i), puweight);
			//  cout << hardjets.at(i)->genPartonFlavour() << endl;
			if(medjets.at(i)->btag() < 0.244) continue;
			medbjets.push_back(medjets.at(i));
		}

		float jetpt_phi=dphillj*dphillj*30/(M_PI*M_PI);
		vector<NTJet*> dphilljjets;
		for(size_t i=0;i<idjets.size();i++){
			if(idjets.at(i)->pt()<jetpt_phi) continue;
			dphilljjets << idjets.at(i);
		}

		vector<NTJet*> dphiplushardjets=mergeVectors(hardjets,dphilljjets);
		std::sort(dphiplushardjets.begin(),dphiplushardjets.end(), comparePt<ztop::NTJet*>);

		evt.dphilljjets=&dphilljjets;
		evt.dphiplushardjets=&dphiplushardjets;

		PolarLorentzVector H4;
		for(size_t i=0;i<idjets.size();i++)
			H4+=idjets.at(i)->p4();
		float ht=H4.Pt();
		evt.ht=&ht;

		PolarLorentzVector S4=leadingptlep->p4() + secleadingptlep->p4() -H4;
		evt.S4=&S4;

		PolarLorentzVector allobjects4=S4+ H4 + H4;
		evt.allobjects4=&allobjects4;


		sel_step[3]+=puweight;
		plots.makeControlPlots(step);
		zplots.makeControlPlots(step);
		////////////////////Z Selection//////////////////
		step++;
		bool isZrange=false;

		if(mll > 76 && mll < 106){

			unf_Zpt.fillReco(dilp4.pt(),puweight);

			isZrange=true;

		}

		bool Znotemu=isZrange;
		if(b_emu_) Znotemu=false;

		////////////////////Z Veto Cut STEP 4 (incl. hard jets)////////////////////////////////////



		if(!Znotemu){

			plots.makeControlPlots(step);
			sel_step[4]+=puweight;

		}
		if(isZrange){
			zplots.makeControlPlots(step);

		}


		///////////////////// at least one jet cut STEP 5 ////////////
		step++;

		if(hardjets.size() < 1) continue;


		//ht+=adjustedmet.met();
		//double mllj=0;
		//double phijl=0;


		if(!Znotemu){

			sel_step[5]+=puweight;
			plots.makeControlPlots(step);


		}
		if(isZrange){
			zplots.makeControlPlots(step);
		}



		/////////////////////// at least two jets STEP 6 /////////////
		step++;


		if(hardjets.size() < 2) continue;
		//if(!midphi && medjets.size()<2) continue;
		//if(ptllj<140) continue;
		/*if(dphillj > 2.65 && dphillj < 3.55){
			if(ptllj < 190)
				continue;
		} */

		if(!Znotemu){

			plots.makeControlPlots(step);
			sel_step[6]+=puweight;
		}
		if(isZrange){

			zplots.makeControlPlots(step);
		}


		//////////////////// MET cut STEP 7//////////////////////////////////
		step++;
		if(!b_emu_ && adjustedmet.met() < 40) continue;




		if(!Znotemu){

			plots.makeControlPlots(step);
			sel_step[7]+=puweight;
		}
		if(isZrange){
			zplots.makeControlPlots(step);

		}

		//	puweight*=0.5;

		///////////////////// btag cut STEP 8 //////////////////////////
		step++;
		if(hardbjets.size() < 1) continue;
		//if(leadingjetbtag<0.244) continue; //cut on leading pt jet

		double mlb=(leadingptlep->p4()+hardbjets.at(0)->p4()).M();



		double bsf=1;//getBTagSF()->getSF(hardjets); // returns 1 for data!!!
		bsf=getBTagSF()->getNTEventWeight(hardjets);
		//  if(bsf < 0.3) cout << bsf << endl;
		puweight= puweight * bsf;

		if(!Znotemu){
			NTLepton * recolep_plus=0, * recolep_minus=0;
			if(leadingptlep->q()>0){
				recolep_plus=leadingptlep;
				recolep_minus=secleadingptlep;
			}
			else{
				recolep_plus=secleadingptlep;
				recolep_minus=leadingptlep;
			}

			plots.makeControlPlots(step);
			sel_step[8]+=puweight;

			unf_tot8.fillReco(1,puweight);

			unf_ivanmlb8.fillReco(mlb,puweight);
			unf_mlb8.fillReco(mlb,puweight);
			unf_mlbfb8.fillReco(mlb,puweight);
			unf_mlbbbb8.fillReco(mlb,puweight);

			//leptons

			unf_ptl.fillReco(recolep_plus->pt(),puweight);
			unf_ptl.fillReco(recolep_minus->pt(),puweight);

			unf_ptll.fillReco(leadingptlep->pt()+secleadingptlep->pt(),puweight);
			unf_Ell.fillReco(leadingptlep->E()+secleadingptlep->E(),puweight);
			unf_dilpt.fillReco((leadingptlep->p4()+secleadingptlep->p4()).Pt(),puweight);
			unf_mll.fillReco((leadingptlep->p4()+secleadingptlep->p4()).M(),puweight);


		}
		if(isZrange){
			zplots.makeControlPlots(step);
		}
		///////////////////// 2btag cut STEP 9 //////////////////////////
		step++;
		if(hardbjets.size() < 2) continue;



		if(!Znotemu){
			plots.makeControlPlots(step);

		}
		if(isZrange){
			zplots.makeControlPlots(step);
		}

	}//main event loop ends
	//if(showstatusbar_)std::cout << std::endl; //for status bar
	container1D::c_makelist =false;
	container1DUnfold::flushAllListed();

	if(testmode_ )
		std::cout << "testmode("<< anaid << "): finished main loop" << std::endl;


	// Fill all containers in the stackVector

	// std::cout << "Filling containers to the Stack\n" << std::endl;
	btagsf_.makeEffs(); //only does that if switched on, so safe

	if(testmode_ )
		std::cout << "testmode("<< anaid << "): prepared b-tag eff" << std::endl;

	// delete t;
	f->Close(); //deletes t
	delete f;
	/*s
	for(size_t i=0;i<ntestconts;i++)
			delete testconts.at(i);
	 */

	///////////////////////////////
	///////////////////////////////
	///////////////////////////////
	///////////////////////////////
	//     WRITE OUTPUT PART     //
	///////////////////////////////
	///////////////////////////////
	///////////////////////////////
	///////////////////////////////

	if(!singlefile_)
		p_askwrite.get(anaid)->pwrite(anaid);
	//std::cout << anaid << " (" << inputfile << ")" << " asking for write permissions to " <<getOutPath() << endl;
	int canwrite=0;
	if(!singlefile_)
		canwrite=p_allowwrite.get(anaid)->pread();
	if(canwrite>0 || singlefile_){ //wait for permission

		if(testmode_ )
			std::cout << "testmode("<< anaid << "): allowed to write to file " << getOutPath()+".root"<< std::endl;

		TFile * outfile;

		if(!fileExists((getOutPath()+".root").Data())) {
			outfile=new TFile(getOutPath()+".root","RECREATE");
		}
		else{
			outfile=new TFile(getOutPath()+".root","READ");
		}

		TTree * outtree;
		ztop::container1DStackVector * csv=&analysisplots_;

		// std::cout << "trying to get tree" <<std::endl;

		if(outfile->Get("stored_objects")){
			outtree=(TTree*)outfile->Get("stored_objects");
			if(outtree->GetBranch("allContainerStackVectors")){ //exists already others are filled
				// csv->loadFromTree(outtree,csv->getName()); //makes copy
				csv=csv->getFromTree(outtree,csv->getName());
			}
		}

		csv->addList(legendname,color,norm,legord);
		if(testmode_ )
			std::cout << "testmode("<< anaid << "): added 1D List"<< std::endl;
		csv->addList2D(legendname,color,norm,legord);
		if(testmode_ )
			std::cout << "testmode("<< anaid << "): added 2D List"<< std::endl;
		csv->addList1DUnfold(legendname,color,norm,legord);
		if(testmode_ )
			std::cout << "testmode("<< anaid << "): added 1DUnfold List"<< std::endl;

		if(issignal)
			csv->addSignal(legendname);

		outfile->Close();
		delete outfile;

		outfile=new TFile((getOutPath()+".root"),"RECREATE");
		outfile->cd();

		outtree= new TTree("stored_objects","stored_objects");
		outtree->Branch("allContainerStackVectors",&csv);
		if(testmode_ )
			std::cout << "testmode("<< anaid << "): added Branch"<< std::endl;
		outtree->Fill();
		if(testmode_ )
			std::cout << "testmode("<< anaid << "): filled new outfile tree"<< std::endl;

		outtree->Write("",TObject::kOverwrite);//"",TObject::kOverwrite);
		if(testmode_ )
			std::cout << "testmode("<< anaid << "): written new outfile tree"<< std::endl;

		outfile->Close();
		delete outfile;

		if(testmode_ )
			std::cout << "testmode("<< anaid << "): written main output"<< std::endl;


		///btagsf
		if(btagsf_.makesEff()){
			ztop::NTBTagSF  btsf;
			TFile * bsffile;
			if(!fileExists(outdir_+btagsffile_.Data())) {
				bsffile=new TFile(outdir_+btagsffile_,"RECREATE");
			}
			else{
				bsffile=new TFile(outdir_+btagsffile_,"READ");
				if(bsffile->Get("stored_objects")){
					TTree * btt = (TTree*) bsffile->Get("stored_objects");
					if(btt->GetBranch("allbTagBase")){
						btsf.readFromTFile(outdir_+btagsffile_);
						//ztop::NTBTagSF  btsf2=btagsf_ + btsf;
						btagsf_ = (btagsf_ + btsf); //combine both if sample already exists, couts warning but doesn't change anything
					}
				}
			}
			bsffile->Close();
			delete bsffile;
			btagsf_.writeToTFile(outdir_+btagsffile_); //recreates the file

		}///makes eff

		std::cout << inputfile << ": " << std::endl;
		for(unsigned int i=0;i<9;i++){
			std::cout << "selection step "<< toTString(i)<< " "  << sel_step[i];
			if(i==3)
				cout << "  => sync step 1 \tmll>20";
			if(i==4)
				cout << "  => sync step 2 \tZVeto";
			if(i==6)
				cout << "  => sync step 3 \t2 Jets";
			if(i==7)
				cout << "  => sync step 4 \tMET";
			if(i==8)
				cout << "  => sync step 5 \t1btag";
			std::cout  << std::endl;
		}
		std::cout << "factor for extrapolation (single dataset genentries/tree-entries) " << genentries << "/" << nEntries <<std::endl;
		std::cout << "count_samesign pairs at dilepton selection: " << count_samesign <<std::endl;

		if(singlefile_) return;

		//all operations done
		p_finished.get(anaid)->pwrite(1); //turns of write blocking, too
	}
	else{
		std::cout << "testmode("<< anaid << "): failed to write to file " << getOutPath()+".root"<< std::endl;
		p_finished.get(anaid)->pwrite(-2); //write failed
	}




}

//- 99: exception -1: file not found, -2 write failed

bool MainAnalyzer::checkTrigger(std::vector<bool> * p_TriggerBools,ztop::NTEvent * pEvent, bool isMC,size_t anaid){


	//do trigger stuff - onlye 8TeV for now
	if(!is7TeV_){
		if(p_TriggerBools->size() < 3)
			return false;

		if(b_mumu_){
			if(!(p_TriggerBools->at(1) || p_TriggerBools->at(2)))
				return false;
		}
		else if(b_ee_){
			if(!p_TriggerBools->at(0))
				return false;
		}
		else if(b_emu_){
			if(p_TriggerBools->size()<10){
				p_finished.get(anaid)->pwrite(-3);
				return false;
			}
			if(!(p_TriggerBools->at(10) || p_TriggerBools->at(11)))
				return false;
		}
	}
	else{ //is7TeV_
		if(p_TriggerBools->size() < 3)
			return false;

		if(b_mumu_){
			if(isMC && !p_TriggerBools->at(5))
				return false;
			if(!isMC && pEvent->runNo() < 163869 && !p_TriggerBools->at(5))
				return false;
			if(!isMC && pEvent->runNo() >= 163869 && !p_TriggerBools->at(6))
				return false;
		}
		else if(b_ee_){
			if(!(p_TriggerBools->at(3) || p_TriggerBools->at(4) || p_TriggerBools->at(1)))
				return false;
		}
		else if(b_emu_){
			std::cout << "emu channel at 7TeV not supported yet (triggers missing)" << std::endl;
			p_finished.get(anaid)->pwrite(-4);
			return false;

			if(p_TriggerBools->size()<10){
				p_finished.get(anaid)->pwrite(-3);
				return false;
			}
			if(!(p_TriggerBools->at(10) || p_TriggerBools->at(11)))
				return false;
		}
	}
	return true;
}



#endif /* EVENTLOOP_H_ */
