#env python

import os,sys
import re
from subprocess import check_output, call


#X509_USER_PROXY=/tmp/x509up_u45059
#proxy=os.environ['X509_USER_PROXY']
#proxy=/tmp/x509up_u45059
proxy="/tmp/x509up_u"+ check_output("id -u",shell=True).split()[0]

print "-> Using proxy",proxy

## we need env -i because there is a mess up with libraries
EOSLS="/usr/bin/eos %(instance)s ls"
GFAL="env -i X509_USER_PROXY="+proxy +" gfal-ls -t 180"
GFAL_EOS="gsiftp://eoscmsftp.cern.ch//eos/cms/store/user/%(user)s"
EOS="/eos/cms/store/user/%(user)s"
EOSData="/eos/cms/store/group/phys_higgs/%(user)s/setup80x_ichep/Data"
EOSMoriond="/eos/cms/store/group/phys_exotica/monojet/%(user)s/moriond80x"
MIT="srm://t3serv006.mit.edu:8443/srm/v2/server?SFN=/mnt/hadoop/cms/store/user/%(user)s"
GFAL_BOX="gsiftp://eosuserftp.cern.ch/eos/user/%(firstletter)s/%(user)s"
BOX="root://eosuser /eos/user/%(firstletter)s/%(user)s"
GFAL_HIGGS="gsiftp://eoscmsftp.cern.ch//eos/cms/store/group/phys_higgs/%(user)s"
HIGGS="/eos/cms/store/group/phys_higgs/%(user)s"

sites={"mit":MIT,"eos":EOS,"eosuser":BOX,"higgs":HIGGS,"eos-data":EOSData,"eos-moriond80x":EOSMoriond}


## association is USER -> SITES 
store={ 
	"amarini":["mit","eos","eosuser"],
	"zdemirag":["mit","eos","eosuser","eos-moriond80x"],
	"ceballos":["mit","eos","eosuser","higgs","eos-data"],
	"dhsu":["mit","eos","eosuser"],
	"mpedraza":["eos","eosuser"],
	"jaeyserm":["eos","eosuser"],
	"klute":["eos","eosuser"],
	}

#replace={"user":"amarini","firstletter":"a"}

output={"tag":[("site","user","dataset")]}

for user in store:
    print "-> Considering",user
    for s in store[user]:
            print "--> Considering",s
	    replace={ "user": user ,"firstletter":user[0:1],"instance":"" }
	    if '/eos/user' in sites[s]: replace["instance"]="root://eosuser"
	    LS=EOSLS[:]
	    if 'gsiftp' in sites[s] or 'srm://' in sites[s]: 
		    LS=GFAL[:]
		

            cmd = LS + " " + sites[s] + "/Nero"
	 
	    try:
                print "   * CMD is",cmd%replace
            	tags= check_output( cmd%replace,shell=True ) . split()
	    except:
		    print "   * not found"
		    continue
	    print "    * found tags:",tags
	    for t in tags:
            	cmd =LS + " " + sites[s] + "/Nero/" + t
            	dirs= check_output( cmd%replace,shell=True ) . split()
		for d in dirs:
			if 'crab3check' in d : continue ## this are the checkwrite tests
			if t not in output: output[t] = []
			output[t] . append( (s,user,d) )

print "------------------------------------------"
for t in output:
	print "TAG = ",t
	for s,u,d in output[t]:
		print " * ",s,u,d
	print "------------------------------------------"
