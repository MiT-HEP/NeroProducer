from NeroProducer.Nero.CMSSWHandler import *
from optparse import OptionParser
import os,sys,re

parser=OptionParser()

parser.add_option("-v","--version",help="VERSION you want to checkout [%default]",default ="CMSSW_7_6_4")
parser.add_option("-d","--destination",help="destination [%default]", default="/afs/cern.ch/work/" + os.environ['USER'][0] + "/" + os.environ['USER'] )
parser.add_option("-t","--tag",help="tag [%default]", default="master" )
parser.add_option("-f","--force",action='store_true',help="force area removal if exists [%default]", default=False )
parser.add_option("","--download",help='''download extra stuff in directory comma separated  [%default].
		Example:
		NeroProducer/Nero/test,https://gist.github.com/amarini/a60d90c2049178954ec22a59b8c214fd/archive/3da5590f8108a0c9e4aec671bb32389ff0c01048.zip
		''', default="" )
parser.add_option("-m","--merge",type='string',
			help='''merge branches (comma separated)  [%default]
			Example:
			amarini:topic_80X
			''', default="" )

opts, args = parser.parse_args()

fullpath=opts.destination
if fullpath[-1] != "/" : fullpath += "/"
fullpath+= opts.version

if os.path.isdir(fullpath) or os.path.isfile(fullpath):
	print "path '" + fullpath +"' exists." 
	if not opts.force:
		print "Refusing to continue!"
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

for pr in opts.merge.split(','):
	origin=pr.split(":")[0]
	branch=pr.split(":")[1]
	print handler.cyan + "-> Merge PR" + handler.white
	handler.testPR(origin,branch)
	if not handler.success() :
		print "* " + handler.red + "ERROR" + handler.white
		exit(0)
	else:
		print "* " + handler.green + "OK" + handler.white

print handler.cyan + "-> SET UP" + handler.white
handler.callSetup("./Nero/script/setup.sh",opts.version,opts.tag)
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

if opts.download != "":
	tocd = opts.download.split(',')[0]
	todownload = opts.download.split(',')[1]
	filename = re.sub("^.*/","",todownload)
	print handler.cyan + "-> DOWNLOADING ADDITIONAL FILES" + handler.white
	cmd = handler._cd + handler._cmsenv 
	cmd += "cd " + tocd +  " &&"
	cmd += "wget "
	cmd += todownload + " && "
	#cmd += "https://gist.github.com/amarini/a60d90c2049178954ec22a59b8c214fd/archive/3da5590f8108a0c9e4aec671bb32389ff0c01048.zip &&"
	if '.zip' in filename: cmd += "unzip -j " ## junk path
	elif '.tgz' in filename or '.tar.gz' in filename: cmd += "tar -xzf "
	cmd += filename

	status = call(cmd,shell=True)
	if status == 0: print "* " + handler.green + "OK" + handler.white
	else: print "* " + handler.red + "ERROR" + handler.white


