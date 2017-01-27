import os
from subprocess import check_output

import FWCore.ParameterSet.Config as cms

neroTag = cms.PSet(
    head = cms.string( check_output("cd "+os.environ['CMSSW_BASE']+"/src/NeroProducer/ && git rev-parse HEAD && cd - 2>&1 >/dev/null",shell=True) ) ,
    tag  = cms.string( check_output("cd " +os.environ["CMSSW_BASE"] +"/src/NeroProducer && { git describe --tags || true ; } && cd - 2>&1 >/dev/null",shell=True) )
)
