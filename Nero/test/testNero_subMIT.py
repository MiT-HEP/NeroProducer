import sys
from FWCore.ParameterSet.VarParsing import VarParsing

cmdopts = VarParsing('analysis')
cmdopts.register('isData', False, VarParsing.multiplicity.singleton, VarParsing.varType.bool, "True if running on Data, False if running on MC")
cmdopts.register('nerohead', "XXX", VarParsing.multiplicity.singleton,VarParsing.varType.string,"Set to the head of the repository. use check_output 'git rev-parse HEAD' in the crab py file. active only if isGrid.")
cmdopts.register("nerotag", "YYY", VarParsing.multiplicity.singleton,VarParsing.varType.string,"Set to the tag of the repository. use check_output 'git rev-parse HEAD' in the crab py file. active only if isGrid.")
cmdopts.register('connect', '', VarParsing.multiplicity.singleton, mytype = VarParsing.varType.string, info = 'Globaltag connect')
cmdopts.register('lumiList', '', VarParsing.multiplicity.singleton, mytype = VarParsing.varType.string, info = 'Good lumi JSON')
cmdopts._tags.pop('numEvent%d')
cmdopts._tagOrder.remove('numEvent%d')

cmdopts.parseArguments()

sys.argv = sys.argv[:1] + [
    'isGrid=True',
    'isData=' + str(cmdopts.isData),
    'nerohead=' + cmdopts.nerohead,
    'nerotag=' + cmdopts.nerotag,
    'maxEvents=' + str(cmdopts.maxEvents)
]
for f in cmdopts.inputFiles:
    sys.argv.append('inputFiles=' + f)

execfile('testNero.py')

if cmdopts.lumiList:
    from FWCore.PythonUtilities.LumiList import LumiList
    process.source.lumisToProcess = LumiList(filename = cmdopts.lumiList).getVLuminosityBlockRange()

if cmdopts.connect:
    if cmdopts.connect == 'mit':
        cmdopts.connect = 'frontier://(proxyurl=http://squid.cmsaf.mit.edu:3128)(proxyurl=http://squid1.cmsaf.mit.edu:3128)(proxyurl=http://squid2.cmsaf.mit.edu:3128)(serverurl=http://cmsfrontier.cern.ch:8000/FrontierProd)/CMS_CONDITIONS'

    # impl taken from PhysicsTools.PatAlgos.tools.helpers
    
    class MassSearchReplaceAnyInputTagVisitor(object):
        """Visitor that travels within a cms.Sequence, looks for a parameter and replace its value
           It will climb down within PSets, VPSets and VInputTags to find its target"""
        def __init__(self,paramSearch,paramReplace,verbose=False):
            self._paramSearch  = paramSearch
            self._paramReplace = paramReplace
            self._moduleName   = ''
            self._verbose=verbose
    
        def doIt(self,pset,base):
            if isinstance(pset, cms._Parameterizable):
                for name in pset.parameterNames_():
                    # if I use pset.parameters_().items() I get copies of the parameter values
                    # so I can't modify the nested pset
                    value = getattr(pset,name)
                    type = value.pythonTypeName()
                    if type == 'cms.PSet':
                        self.doIt(value,base+"."+name)
                    elif type == 'cms.VPSet':
                        for (i,ps) in enumerate(value): self.doIt(ps, "%s.%s[%d]"%(base,name,i) )
                    elif type == 'cms.vstring':
                        for (i,n) in enumerate(value):
                             if (n == self._paramSearch):
                                if self._verbose:print "Replace %s.%s[%d] %s ==> %s " % (base, name, i, self._paramSearch, self._paramReplace)
                                value[i] = cms.string(self._paramReplace)
    
                    elif type.endswith('.string'):
                        if value.value() == self._paramSearch:
                            if self._verbose:print "Replace %s.%s %s ==> %s " % (base, name, self._paramSearch, self._paramReplace)
                            if 'untracked' in type:
                                setattr(pset, name, cms.untracked.string(self._paramReplace))
                            else:
                                setattr(pset, name, cms.string(self._paramReplace))
    
        def enter(self,visitee):
            label = ''
            try:
                label = visitee.label_()
            except RuntimeError:
                label = '<unlabeled>'

            self.doIt(visitee, label)

        def leave(self,visitee):
            pass
    
    
    replacer = MassSearchReplaceAnyInputTagVisitor('frontier://FrontierProd/CMS_CONDITIONS', cmdopts.connect, True)

    process.p.visit(replacer)
    for n, s in process.es_sources.items():
        replacer.doIt(s, n)
    for n, s in process.es_producers.items():
        replacer.doIt(s, n)
    for n, s in process.es_prefers.items():
        replacer.doIt(s, n)
