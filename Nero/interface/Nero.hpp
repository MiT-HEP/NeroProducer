// Original Author:  Andrea Carlo Marini
//         Created:  Tue, 19 May 2015 13:38:17 GMT
//
#ifndef NERO_H
#define NERO_H

#include "NeroProducer/Nero/interface/Includes.hpp"
#include "NeroProducer/Nero/interface/NeroCollection.hpp"
#include "NeroProducer/Nero/interface/NeroRunLumi.hpp"

#include "TStopwatch.h"

//fwd declaration

//
// class declaration
//

class Nero : public edm::EDAnalyzer {
    public:
        explicit Nero(const edm::ParameterSet&);
        ~Nero();

        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


    private:
        virtual void beginJob() override;
        virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
        virtual void endJob() override;

        virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
        virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
        virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
        virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

        void respondToOpenInputFile(edm::FileBlock const&) override;
        // ----------member data ---------------------------
        TTree *tree_;
        TTree *all_;
        TTree *files_; // saved analized file Names
        string *fileName;

        TH1F  *hXsec_;
        map<string,TH1D*> hD_;

        edm::Service<TFileService> fileService_;
        // collection to be moved into a tree
        vector<NeroCollection*> obj;
        vector<NeroRun*> runObj;
        vector<NeroLumi*> lumiObj;

        // check performances
        TStopwatch sw_;
        map<string,float> times_;

        //NeroJets    *jets_;
        //NeroTaus	  *taus_;
        //NeroLeptons *leps_;
        
        // GIT TAG AND INFO
        string tag_; // last tag
        string head_; // head
        string info_; // bare config, ch Higgs, monojet
        string cmssw_; //
};



#endif
// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
