import FWCore.ParameterSet.Config as cms

TriggerFilter = cms.EDFilter("TriggerFilter",
		trigger=cms.InputTag("TriggerResults","","HLT"),
		triggerNames=cms.vstring(["HLT_IsoMu20_v","HLT_IsoMu24_v","HLT_IsoMu27_v",
			"HLT_LooseIsoPFTau50_Trk30_eta2p1", ## this should already catch them all
			"HLT_LooseIsoPFTau50_Trk30_eta2p1_MET80",
			"HLT_LooseIsoPFTau50_Trk30_eta2p1_MET90"
			])
		)
