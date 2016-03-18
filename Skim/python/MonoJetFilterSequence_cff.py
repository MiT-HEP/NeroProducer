import FWCore.ParameterSet.Config as cms

from NeroProducer.Skim.MonoJetFilter_cfi import *

monojetFilterSequence     = cms.Sequence( MonoJetFilter )
