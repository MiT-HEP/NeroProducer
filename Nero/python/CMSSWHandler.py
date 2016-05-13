import os,sys
import re
from subprocess import call,check_output


class CMSSWHandler:
    ''' Handle for a CMSSW Area '''
    def __init__(self,repo="MiT-HEP/NeroProducer"):
        self.tmpdir = "/tmp/" + os.environ['USER']
        self.version = "CMSSW_7_6_3_patch2"
        self.repo = repo

        #### DONT CHANGE
        ## colors
        self.red="\033[01;31m"
        self.green = "\033[01;32m"
        self.yellow = "\033[01;33m"
        self.blue = "\033[01;34m"
        self.pink = "\033[01;35m"
        self.cyan = "\033[01;36m"
        self.white = "\033[00m"
        self.verbose = True
        self.error = 0 ## last error, if != 0
        self.debug = True
        self.ntries = 3
    
    def _remove(self):
        if self.debug: print "* Removing CMSSW"
        cmd = "cd %s &&"% self.tmpdir
        cmd += "rm -rf %s"% self.version

        call(cmd ,shell=True) ## don't check
        return self

    def _cmsrel(self):
        ## cmsrel
        ## TODO, implement scram project, to change the target name
        if self.debug: print "* Creating new CMSSW"
        cmd = "cd %s &&"% self.tmpdir
        #cmd += "cmsrel %s" %self.version
        cmd += "/cvmfs/cms.cern.ch/common/scramv1 project CMSSW  %s" %self.version
        self.error=call(cmd,shell=True)
        return self

    def _build(self):
        cmd = self._cd + self._cmsenv
        cmd += "scram b -j 16"

        if self.debug: print "* Calling a build with cmd '"+ cmd + "'"

        return call(cmd,shell=True)


    def initArea(self):
        if self.verbose: print self.cyan+"-> Init new CMSSW area"+self.white
        self._remove()

        self._cmsenv = " eval `scramv1 runtime -sh` &&"
        self._cd = "cd %s/%s/src &&" %(self.tmpdir, self.version)
        self._fetched=[]

        self._cmsrel()
        if self.error !=0 : return self

        if self.debug: print "* Calling git cms-init"+self.white
        ## init cmssw
        cmd = self._cd + self._cmsenv
        if self.debug: cmd += " pwd && git config -l  && "
        cmd += " git cms-init "
        if self.debug: print "     with  cmd '"+cmd +"'"
        self.error = call(cmd,shell=True)
        if self.error !=0 : return self

        ## add packages update TODO git cms-addpgk
        if self.debug: print "* Clonig repositories"+self.white
        for idx,repo in enumerate(self.repo.split(',')):
            if self.error != 0: return 
            ## repo = MiT-HEP/NeroProducer
            if idx==0: self._repomain = repo.split('/')[1] 

            cmd = self._cd + self._cmsenv
            cmd += "git clone git@github.com:"+repo
            self.error = call( cmd, shell=True)
        return self

    def callSetup(self,setup="./Nero/script/setup.sh",args="",sha=""):
        if self.verbose: print self.cyan+"-> Call setup script: "+self.white + setup + " " + args + " @" + sha
        cmd = self._cd + self._cmsenv
        cmd += "cd " + self._repomain + " &&"
        cmd += "chmod u+x " + setup + " && "
        if sha != "":
            cmd +=  "git checkout " + sha + " && "
        cmd += setup + " " + args

        if self.debug : print "* with cmd '" +cmd +"'"

        self.error = call(cmd, shell=True)
        return self

    def build(self):
        if self.verbose: print self.cyan+"-> Build" 
        self.error=-1
        ntry=1
        while self.error !=0 and ntry <= self.ntries:
            if self.debug: print "* try",ntry,"of",self.ntries
            self.error = self._build()
            ntry+=1
        return self

    def testPR(self,origin,sha):
        if self.verbose:print self.cyan+"-> Merging PR"+self.white
        name = origin.split('/')[0]
        cmd = self._cd + self._cmsenv
        cmd += "cd %s &&" % self._repomain
        if name not in self._fetched:
            cmd += "git remote add %s git@github.com:%s && "%(name,origin)
            self._fetched.append(name)

        cmd += "git fetch %s && "%name
        cmd += "git merge --no-ff -m 'merge commit' %s  "%sha
        self.error = call(cmd,shell=True)
        return self

    def success(self):
        if self.error == 0 : return True
        return False



# Local Variables:
# mode:python
# indent-tabs-mode:nil
# tab-width:4
# c-basic-offset:4
# End:
# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
