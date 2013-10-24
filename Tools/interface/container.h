#ifndef container_h
#define container_h

#include <vector>
#include "TH1D.h"
#include "TGraphAsymmErrors.h"
#include <iostream>
#include <math.h>
#include <utility>
#include "TPad.h"
#include "TopAnalysis/ZTopUtils/interface/miscUtils.h"
#include <algorithm>
#include <complex>
#include "histoContent.h"

////////bins include their border on the right side!!



namespace ztop{
class container2D;

class container1D{
	friend class container2D;
public:
	container1D();
	container1D(float , TString name="",TString xaxisname="",TString yaxisname="", bool mergeufof=false);              //! construct with bin width (for dynamic filling - not yet implemented)
	container1D(std::vector<float> , TString name="",TString xaxisname="",TString yaxisname="", bool mergeufof=false); //! construct with binning
	~container1D();
	container1D(const container1D&);
	container1D& operator=(const container1D&);


	const TString & getName()const{return name_;}
	void setName(TString Name){name_=Name;}
	void setNames(TString name, TString xaxis, TString yaxis){if(name!="")name_=name;if(xaxis!="")xname_=xaxis;if(yaxis!="")yname_=yaxis;}
	void setShowWarnings(bool show){showwarnings_=show;}

	void fill(const double & val, const double & weight=1);    //! fills with weight
	void fillDyn(double what, double weight=1); //! not implemented yet

	void setBins(std::vector<float>);
	void setBinWidth(float);            //! sets bin width; enables dynamic filling (not implemented yet)

	void setBinErrorUp(const size_t&, const double&);    //! clears ALL systematics and adds a new one
	void setBinErrorDown(const size_t&, const double&);  //! clears ALL systematics and adds a new one
	void setBinError(const size_t&, const double&);      //! clears ALL systematics and adds a new one

	void setBinContent(const size_t&, const double&,const int &sysLayer=-1);    //! sets bin content, does not change error values!
	void setBinStat(const size_t &bin, const double & err, const int &sysLayer=-1); //! sets bin stat, does not change error values!
	void setBinEntries(const size_t &bin, const size_t & entries, const int &sysLayer=-1); //! sets bin stat, does not change error values!


	size_t getBinNo(const double&) const; //! returns bin index number for (double variable)
	size_t getNBins() const;       //! returns nuberof bins
	const std::vector<float> & getBins() const {return bins_;}

	const histoBin & getBin(const size_t & no, const int& layer=-1)const{return contents_.getBin(no,layer);}
	histoBin & getBin(const size_t & no, const int& layer=-1){return contents_.getBin(no,layer);}

	double getBinCenter(const size_t &) const;
	double getBinWidth(const size_t &) const; //! returns total bin width NOT 0.5* width

	const float &getXMax() const{return bins_[getNBins()+1];}
	const float &getXMin() const{return bins_[1];}

	 size_t getSystSize() const{return contents_.layerSize();}

	const double & getBinContent(const size_t&,const int &sysLayer=-1) const;
	const size_t & getBinEntries(const size_t &,const int &sysLayer=-1) const;
	 double  getBinStat(const size_t &,const int &sysLayer=-1) const;

	double  getBinErrorUp(const size_t&, bool onlystat=false,const TString& limittosys="") const; //returns DEVIATION!
	double  getBinErrorDown(const size_t&,bool onlystat=false,const TString&  limittosys="") const; //returns DEVIATION!
	double getBinError(const size_t&,bool onlystat=false,const TString&  limittosys="") const; //returns DEVIATION!

	double  getSystError(const size_t & number, const size_t & bin) const; //returns DEVIATION!
	double getSystErrorStat(const size_t & number, const size_t & bin) const; //returns DEVIATION!

	const TString & getSystErrorName(const size_t & number) const;
	const size_t & getSystErrorIndex(const TString & ) const;

	container1D breakDownSyst(const size_t & bin, bool updown) const;
	container1D sortByBinNames(const container1D & ) const;

	double getOverflow(const int& systLayer=-1) const;                              //!returns -1 if overflow was merged with last bin
	double getUnderflow(const int& systLayer=-1) const;                             //!returns -1 if underflow was merged with last bin

	double integral(bool includeUFOF=false,const int& systLayer=-1) const;
	double cIntegral(float from=0, float to=0,const int& systLayer=-1) const;       //! includes the full bin "from" or "to" is in

	double integralStat(bool includeUFOF=false,const int& systLayer=-1) const;
	double cIntegralStat(float from=0, float to=0,const int& systLayer=-1) const;       //! includes the full bin "from" or "to" is in

	size_t integralEntries(bool includeUFOF=false,const int& systLayer=-1) const;
	size_t cIntegralEntries(float from=0, float to=0,const int& systLayer=-1) const;       //! includes the full bin "from" or "to" is in

	double getYMax(bool dividebybinwidth ,const int& systLayer=-1) const;
	double getYMin(bool dividebybinwidth, const int& systLayer=-1) const;

	double normalize(bool includeUFOF=true, const double &normto=1);


	void reset();    //! resets all uncertainties and binning, keeps names and axis
	void clear();    //! sets all bin contents to zero; clears all systematic uncertainties

	void setLabelSize(double size){labelmultiplier_=size;}       //! 1 for default
	TH1D * getTH1D(TString name="", bool dividebybinwidth=true, bool onlystat=false) const; //! returns a TH1D pointer with symmetrized errors (TString name); small bug with content(bin)=0 and error(bin)=0
	TH1D * getTH1DSyst(TString name, size_t systNo, bool dividebybinwidth=true, bool statErrors=false) const;
	//operator TH1D() const {TH1D* h=getTH1D();TH1D hr=*h;delete h;return hr;}
	// bad practise operator TH1D*(){return getTH1D();}
	//container1D & operator = (const TH1D &);
	container1D & import(TH1 *,bool isbinwidthdivided=false);
	void writeTH1D(TString name="",bool dividebybinwidth=true, bool onlystat=false) const; //! writes TH1D->Write() with symmetrized errors (TString name)

	TGraphAsymmErrors * getTGraph(TString name="", bool dividebybinwidth=true,bool onlystat=false,bool noXErrors=false) const;
	TGraphAsymmErrors * getTGraphSwappedAxis(TString name="", bool dividebybinwidth=true,bool onlystat=false,bool noXErrors=false) const;
	operator TGraphAsymmErrors() const {return *getTGraph();}
	operator TGraphAsymmErrors*() const {return getTGraph();}
	void writeTGraph(TString name="",bool dividebybinwidth=true,bool onlystat=false, bool noXErrors=false) const; //! writes TGraph to TFile (must be opened)

	void drawFullPlot(TString name="", bool dividebybinwidth=true,const TString &extraoptions="");

	void setDivideBinomial(bool);                                   //! default true
	void setMergeUnderFlowOverFlow(bool merge){mergeufof_=merge;}   //! merges underflow/overflow in first or last bin, respectively

	container1D & operator += (const container1D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	container1D operator + (const container1D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	container1D & operator -= (const container1D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	container1D operator - (const container1D &);       //! adds errors in squares; treats same named systematics as correlated!!
	container1D & operator /= (const container1D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	container1D operator / (const container1D &);       //! binomial stat error or uncorr error (depends on setDivideBinomial()); treats same named systematics as correlated
	container1D & operator *= (const container1D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	container1D operator * (const container1D &);       //! adds stat errors in squares; treats same named systematics as correlated!!
	container1D & operator *= (double);            //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
	container1D operator * (double);            //! simple scalar multiplication. stat and syst errors are scaled accordingly!!


	//void mergeBins(size_t ,size_t );
	std::vector<float> getCongruentBinBoundaries(const container1D &) const;
	container1D rebinToBinning(const std::vector<float> & newbins) const;
	container1D rebinToBinning(const container1D &) const;
	container1D rebin(size_t merge) const;

	void addErrorContainer(const TString &,const container1D&, double);  //! adds deviation to (this) as systematic uncertianty with name and weight. name must be ".._up" or ".._down"
	void addErrorContainer(const TString &,const container1D&);        //! adds deviation to (this) as systematic uncertianty with name. name must be ".._up" or ".._down"
	void addRelSystematicsFrom(const container1D &);
	void addGlobalRelErrorUp(const TString &,const double &);    //! adds a global relative uncertainty with name; "..up" automatically added
	void addGlobalRelErrorDown(const TString &,const double &);  //! adds a global relative uncertainty with name; "..down" automatically added
	void addGlobalRelError(const TString &,const double &);      //! adds a global relative symmetric uncertainty with name; creates ".._up" and ".._down" variation names

	void removeError(const TString &); //! removes a systematic uncertainty with name ..
	void removeError(const size_t &); //! removes a systematic uncertainty with idx ..
	void renameSyst(const TString &,const  TString&); //! old, new

	void transformStatToSyst(const TString&);
	void setAllErrorsZero(){contents_.removeAdditionalLayers();contents_.clearLayerStat(-1);} //! sets all errors zero
	void removeAllSystematics(){contents_.removeAdditionalLayers();}

	static bool c_makelist;
	static std::vector<container1D*> c_list;
	static void c_clearlist(){c_list.clear();}
	static bool debug;

	static void setOperatorDefaults();

	void coutFullContent() const;

	bool hasSameLayers(const container1D&) const;
	bool hasSameLayerOrdering(const container1D&) const;

	void coutBinContent(size_t bin) const;

protected:
	bool showwarnings_;
	float binwidth_;
	bool canfilldyn_;

	bool manualerror_;

	std::vector<float> bins_;
	histoContent contents_;
	/*
	std::vector<double> content_;
	std::vector<long int> entries_;
	std::vector<double> staterrup_;
	std::vector<double> staterrdown_;  //has POSITIVE entries

	std::vector<std::pair<TString, std::vector<double> > > syserrors_; //syst (name, each bin) stores absolute DEVIATIONS from nominal!!!!
	 */


	bool mergeufof_;
	bool wasunderflow_;
	bool wasoverflow_;

	bool divideBinomial_;

	TString name_, xname_, yname_;

	double labelmultiplier_;

	TGraphAsymmErrors * gp_;
	TH1D * hp_;

//containerStyle * style_; //if NULL, use some defaults, else use style
	//containerStyle  persstyle_; //only when saving

	TString stripVariation(const TString&) const;
	double getDominantVariationUp( TString ,const size_t &) const;
	double getDominantVariationDown( TString ,const size_t &) const;

	double sq(const double&) const; //helper

	void createManualError();

	void copyFrom(const container1D&);

};

} //namespace


//some more operators
ztop::container1D operator * (double multiplier, const ztop::container1D & cont);
ztop::container1D operator * (float multiplier, const ztop::container1D & cont);  //! simple scalar multiplication. stat and syst errors are scaled accordingly!!
ztop::container1D operator * (int multiplier, const ztop::container1D & cont);    //! simple scalar multiplication. stat and syst errors are scaled accordingly!!

inline double ztop::container1D::sq(const double& a) const{
	return a*a;
}

inline size_t ztop::container1D::getBinNo(const double & var) const{

	if(bins_.size()<1){
		return 0;
	}
	if(bins_.size() <2){
		return 0;
	}
	std::vector<float>::const_iterator it=std::lower_bound(bins_.begin()+1, bins_.end(), var);
	if(var==*it)
		return it-bins_.begin();
	else
		return it-bins_.begin()-1;

}


inline void ztop::container1D::fill(const double & what, const double & weight){
	size_t bin=getBinNo(what);
	//int bin=0;

	if(mergeufof_){
		if(bin==0 && bins_.size() > 1){
			bin=1;
			wasunderflow_=true;
		}
		else if(bin==bins_.size()-1){
			bin--;
			wasoverflow_=true;
		}
	}
	histoBin * b=&contents_.getBin(bin);
	b->addToContent(weight);
	b->addEntry();
	b->addToStat(weight);
}
inline void ztop::container1D::fillDyn(double what, double weight){
	if(canfilldyn_){
		what++;
		weight++; //just avoid warnings function not implemented yet. creates new bins based on binwidth_ if needed
	}
	else if(showwarnings_){
		std::cout << "dynamic filling not available with fixed bins!" << std::endl;
	}
}




#endif
