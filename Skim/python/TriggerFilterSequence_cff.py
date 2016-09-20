import FWCore.ParameterSet.Config as cms

from NeroProducer.Skim.TriggerFilter_cfi import *

triggerFilterSequence     = cms.Sequence( TriggerFilter )
