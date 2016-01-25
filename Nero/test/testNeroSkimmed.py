import FWCore.ParameterSet.Config as cms

process = cms.Process("nero")

process.load("FWCore.MessageService.MessageLogger_cfi")
# If you run over many samples and you save the log, remember to reduce
# the size of the output by prescaling the report of the event number
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )


#fileList = [ "/store/mc/Spring14miniaod/WToTauNu_M_200_Tune4C_tauola_13TeV_pythia8/MINIAODSIM/PU20bx25_POSTLS170_V5-v1/00000/00100400-9007-E411-964D-0025905AC822.root",
#	     "/store/mc/Spring14miniaod/WToTauNu_M_200_Tune4C_tauola_13TeV_pythia8/MINIAODSIM/PU20bx25_POSTLS170_V5-v1/00000/08194701-9007-E411-B181-0025905AC982.root",
#	    ]
fileList = []


### do not remove the line below!
###FILELIST###

process.source = cms.Source("PoolSource",
        fileNames = cms.untracked.vstring(fileList)
        )

# ---- define the output file -------------------------------------------
process.TFileService = cms.Service("TFileService",
        closeFileFast = cms.untracked.bool(True),
        fileName = cms.string("NeroNtuples.root"),
        )

#process.load('NeroProducer.Nero.Nero_cfi')
process.load('NeroProducer.Nero.NeroChargedHiggs_cfi')
#process.load('NeroProducer.Nero.NeroMonojet_cfi')

process.p = cms.Path(
        process.nero
        )

# Local Variables:
# mode:python
# indent-tabs-mode:nil
# tab-width:4
# c-basic-offset:4
# End:
# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
