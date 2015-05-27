#ifndef NERO_LUMI_RUN_H
#define NERO_LUMI_RUN_H

#include "NeroProducer/Nero/interface/Includes.hpp" 
#include "NeroProducer/Core/interface/BareCollection.hpp"

class NeroLumi: virtual public BareCollection{
public:
	// --- Just virtual
	virtual int  analyzeLumi(const edm::LuminosityBlock &,TTree*) {return 0;}
	virtual int  analyzeLumi(const edm::LuminosityBlock &,TH1F*) {return 0;}
	virtual inline string name(){return "NeroLumi";}
};

class NeroRun: virtual public BareCollection{
public:
	// --- Just virtual
	virtual inline int  analyzeRun(const edm::Run &,TTree*) {return 0;}
	virtual inline int  analyzeRun(const edm::Run &,TH1F*) {return 0;} 
	virtual inline string name(){return "NeroRun";}
};

#endif
