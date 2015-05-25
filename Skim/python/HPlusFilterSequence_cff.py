import FWCore.ParameterSet.Config as cms

from NeroProducer.Skim.HPlusFilter_cfi import *

hplusFilterSequence     = cms.Sequence( HPlusFilter )
