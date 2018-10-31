import FWCore.ParameterSet.Config as cms

from NeroProducer.Skim.MuonFilter_cfi import *

muonFilterSequence     = cms.Sequence( MuonFilter )
