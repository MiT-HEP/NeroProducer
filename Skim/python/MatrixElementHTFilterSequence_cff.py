import FWCore.ParameterSet.Config as cms

from NeroProducer.Skim.MatrixElementHTFilter_cfi import *

## put the sequence BEFORE infoProducer, iff cross section normalization is for 0 jets only
## if xsec is for the full dataset put it after
meHTFilterSequence     = cms.Sequence( meHTFilter )
