import FWCore.ParameterSet.Config as cms
from NeroProducer.Nero.Nero_cfi import nero

print " ------- LOADING NERO PHOTONS CONFIGURATION -------- "

nero.info = cms.string("NeroPhotnsjet")

minPhoPt  = cms.double (150.),
minPhoEta = cms.double (2.5),
minPhoN   = cms.int32  (2),
maxPhoIso = cms.double (-1),


# Local Variables:
# mode:python
# indent-tabs-mode:nil
# tab-width:4
# c-basic-offset:4
# End:
# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
