from bambuToNero import mithep, analysis, sequence

monojetSequence = None
for mod in seqeunce:
    if monojetSequence is None:
        monojetSequence = mod
        continue

    if mod._clsName == 'mithep.MetCorrectionMod':
        continue
    elif mod._clsName == 'mithep.NeroMod':
        neroMod = mod
        break

    monojetSequence *= mod

fillers = [f for f in neroMod.GetFillers()]

# need to modify met filler because JES +-1 simga correction is not in the skim
metFiller = mithep.nero.MetFiller(
    MetName = 'PFType1CorrectedMet',
    JESUpMetName = '',
    JESDownMetName = '',
    MuonsName = 'TightMuons',
    GenMetName = 'GenMet'
)

iF = 0
while iF != len(fillers):
    if fillers[iF] and fillers[iF]._clsName = 'mithep.nero.MetFiller':
        fillers[iF] = metFiller
        break

def makeNeroMod(catName, category):
    global mithep
    global fillers

    head = 'HEAD'
    tag = 'BAMBU_041'
    
    neroMod = mithep.NeroMod('Nero' + catName,
        Info = 'Nero',
        Head = head,
        Tag = tag,
        FileName = 'nero_' + catName + '.root',
        PrintLevel = 0
    )
    for filler in fillers:
        neroMod.AddFiller(filler.clone())

    categoryMod = mithep.EventCategoryMod(catName + 'Filter',
        InputName = 'MonoJetEventCategories',
        Category = category
    )

    neroMod.SetCondition(categoryMod)

    return (categoryMod, neroMod)

signalCat, signalFil = makeNeroMod('Signal', mithep.MonoJetAnalysisMod.kSignal)
dielectronCat, dielectronFil = makeNeroMod('Dielectron', mithep.MonoJetAnalysisMod.kDielectron)
dimuonCat, dimuonFil = makeNeroMod('Dimuon', mithep.MonoJetAnalysisMod.kDimuon)
singleelectronCat, singleelectronFil = makeNeroMod('SingleElectron', mithep.MonoJetAnalysisMod.kSingleElectron)
singlemuonCat, singlemuonFil = makeNeroMod('SingleMuon', mithep.MonoJetAnalysisMod.kSingleMuon)
photonCat, photonFil = makeNeroMod('Photon', mithep.MonoJetAnalysisMod.kPhoton)

monojetSequence *= (signalCat + dielectronCat + dimuonCat + singleelectronCat + singlemuonCat + photonCat)
monojetSequence += signalFil + dielectronFil + dimuonFil + singleelectronFil + singlemuonFil + photonFil

analysis.SetAllowNoHLTTree(True)

analysis.setSequence(monojetSequence)
