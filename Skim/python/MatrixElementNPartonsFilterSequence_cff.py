import FWCore.ParameterSet.Config as cms

from NeroProducer.Skim.MatrixElementNPartonsFilter_cfi import *

## put the sequence BEFORE infoProducer, iff cross section normalization is for 0 jets only
## if xsec is for the full dataset put it after
meNPartonsFilterSequence     = cms.Sequence( meNPartonsFilter )
