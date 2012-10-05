#ifndef containerUF_h
#define containerUF_h

#include "containerStack.h"
#include "TString.h"

// container with some more plotting options and (very simple) unfolding

// make containerUFStack class inherits from normal stack?!? (check protected,private etc. such that all functions on member containers_ now apply for a vector of containerUF.. might be tricky / or not? in case of doubt containers_ ->private

//include Alan Caldwells stat error interpretation?? -> check with someone, how they feel about it.

namespace top{

  class container1DUF : public container1D{

  public:

    container1DUF(){};
    container1DUF(const top::container1DStack &, const top::container1DStack &, TString , double , TString dataname="data");
    container1DUF(const top::container1D &);
    ~container1DUF(){};

    void setGenStack(const top::container1DStack &gen){gen_=gen;}
    void setSelectedStack(const top::container1DStack &sel){sel_=sel;}
    void setSignal(TString signalname){signalname_=signalname;}
    void setDataName(TString name){dataname_=name;}

    void simpleUnfold();

    top::container1DUF breakDownSystematicsInBin(int);


    //some plotting stuff - still missing



  private:
    top::container1DStack gen_;
    top::container1DStack sel_;
    double lumi_;
    TString dataname_;
    TString signalname_;
    std::vector<TString> binnames_;

  };


  //functions split here if you like

  container1DUF::container1DUF(const top::container1DStack &gen, const top::container1DStack &sel, TString signalname, double lumi, TString dataname){
    gen_=gen;
    sel_=sel;
    dataname_=dataname;
    signalname_=signalname;
    lumi_=lumi;
    simpleUnfold();
  }

  container1DUF::container1DUF(const top::container1D & cont){ //convert
    showwarnings_=cont.showwarnings_;
    binwidth_=cont.binwidth_;
    bins_=cont.bins_;
    canfilldyn_=cont.canfilldyn_;
    content_=cont.content_;
    entries_=cont.entries_;
    staterrup_=cont.staterrup_;
    staterrdown_=cont.staterrdown_;
    syserrors_=cont.syserrors_;
    mergeufof_=cont.mergeufof_;
    wasunderflow_=cont.wasunderflow_;
    wasoverflow_=cont.wasoverflow_;
    divideBinomial_=cont.divideBinomial_;
    name_=cont.name_;
    xname_=cont.xname_;
    yname_=cont.yname_;
    labelmultiplier_=cont.labelmultiplier_;
  }

  void container1DUF::simpleUnfold(){

    // find signal in gen and sel
    top::container1D gencont=gen_.getContribution(signalname_);
    top::container1D selcont=sel_.getContribution(signalname_);
   
    // get efficiency and background
    std::vector<TString> excludefrombackground;
    excludefrombackground << signalname_ << dataname_;

    //   gencont.setDivideBinomial(false);
    // selcont.setDivideBinomial(false);
    // gencont.getTH1D()->Draw();


    // binomial error for efficiency

    top::container1D efficiency = selcont / gencont;
    top::container1D background = sel_.getContributionsBut(excludefrombackground);
    
    // efficiency.getTH1D()->Draw();

    //get data
    top::container1D data=sel_.getContribution(dataname_);

    top::container1D select = (data - background);
    // select.getTH1D()->Draw();

    // and do the "unfolding"
    data.setDivideBinomial(false);
    background.setDivideBinomial(false);
    efficiency.setDivideBinomial(false);

    efficiency.transformStatToSyst("MC_stat"); //when using multiple of these don't forget to rename!!!

    top::container1D output = (data - background) / efficiency;
      output=output * (1/lumi_);

      //  output.getTH1D()->Draw();

    *this = container1DUF(output);

  }
  top::container1DUF container1DUF::breakDownSystematicsInBin(int bin){
    std::vector<float> sysbins;
    for(unsigned int i=0;i<syserrors_.size();i++){
      sysbins << i;
    }
    top::container1D temp(sysbins,"source","contribution","");
    for(unsigned int i=0;i<syserrors_.size();i++){


    }
    
    return out;
  }

}

#endif
