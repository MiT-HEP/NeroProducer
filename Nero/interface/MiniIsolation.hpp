#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
//Root Classes

#include "TH1F.h"
#include "TH2F.h"
#include "TH1I.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TTree.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TString.h"
#include "TMath.h"
#include "TLorentzVector.h"
#include "TLegend.h"

//Standard C++ classes
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <utility>
#include <ostream>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <memory>
#include <iomanip>

double getPFMiniIsolation_DeltaBeta(edm::Handle<pat::PackedCandidateCollection> pfcands,
			  const reco::Candidate* ptcl,  
			  double r_iso_min, double r_iso_max, double kt_scale,
			  bool charged_only);

double getPFMiniIsolation_EffectiveArea(edm::Handle<pat::PackedCandidateCollection> pfcands,
			  const reco::Candidate* ptcl,  
			  double r_iso_min, double r_iso_max, double kt_scale,
					bool use_pfweight,  bool charged_only, double rho);

