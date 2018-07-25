import FWCore.ParameterSet.Config as cms

bRegressionProducer= cms.EDProducer('bRegressionProducer',
           JetTag=cms.InputTag("slimmedJets"),
           svCollection=cms.InputTag("slimmedSecondaryVertices"),
           vtxCollection=cms.InputTag("offlineSlimmedPrimaryVertices"),
           rhoFixedGridCollection=cms.InputTag("fixedGridRhoFastjetAll"),
           bRegressionWeightfile= cms.untracked.string("NeroProducer/Skim/data/breg_training_2017.pb"), 
           y_mean = cms.untracked.double(1.0454729795455933),
           y_std =cms.untracked.double( 0.31628304719924927)
           )

