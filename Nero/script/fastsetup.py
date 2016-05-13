from NeroProducer.Nero.CMSSWHandler import *
from optparse import OptionParser
import os,sys,re

parser=OptionParser()

parser.add_option("-v","--version",help="VERSION you want to checkout [%default]",default ="CMSSW_7_6_4")
parser.add_option("-d","--destination",help="destination [%default]", default="/afs/cern.ch/work/" + os.environ['USER'][0] + "/" + os.environ['USER'] )
parser.add_option("-t","--tag",help="tag [%default]", default="master" )

opts, args = parser.parse_args()


fullpath=opts.destination
if fullpath[-1] != "/" : fullpath += "/"
fullpath+= opts.version

if os.path.isdir(fullpath) or os.path.isfile(fullpath):
	print "path '" + fullpath +"' exists. Refusing to continue!"
	exit(0)

handler = CMSSWHandler()
handler.tmpdir = opts.destination
handler.version = opts.version

print handler.cyan + "-> INIT AREA" + handler.white
handler.initArea()
if not handler.success() :
	print "* " + handler.red + "ERROR" + handler.white
	exit(0)
else:
	print "* " + handler.green + "OK" + handler.white

print handler.cyan + "-> SET UP" + handler.white
handler.callSetup()
if not handler.success() :
	print "* " + handler.red + "ERROR" + handler.white
	exit(0)
else:
	print "* " + handler.green + "OK" + handler.white


print handler.cyan + "-> BUILD" + handler.white
handler.build()
if not handler.success() :
	print "* " + handler.red + "ERROR" + handler.white
	exit(0)
else:
	print "* " + handler.green + "OK" + handler.white

print "--- "+ handler.green + "DONE" + handler.white + " ---"


