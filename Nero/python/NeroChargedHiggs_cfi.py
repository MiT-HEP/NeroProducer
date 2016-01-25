import FWCore.ParameterSet.Config as cms
from NeroProducer.Nero.Nero_cfi import nero

print " ------- LOADING CHARGED HIGGS CONFIGURATION -------- "

nero.info = cms.string("NeroChargedHiggs")

nero.minJetPt  = cms.double (25.)
nero.minJetEta = cms.double (4.7)

nero.minTauId  = cms.string ('decayModeFindingNewDMs')
nero.maxTauIso = cms.double (-1)
nero.extendTau = cms.bool(True)
nero.extendMet = cms.bool(True)

# Local Variables:
# mode:python
# indent-tabs-mode:nil
# tab-width:4
# c-basic-offset:4
# End:
# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
