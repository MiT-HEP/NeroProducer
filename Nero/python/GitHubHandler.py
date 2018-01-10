import os,sys
import re

import json
import requests

from datetime import datetime
import dateutil.parser

class PullReq:
	''' Handle PR '''
	def __init__(self):
		self.mergeable = False
		self.merged = False
		self.number = 0
		self.title = ""
		self.sha=''
		self.origin=''
		self.labels=[]

	def ParseRequest(self,pr,github=None):
		''' parse a pr json and fill the relevant fields'''
		self.number = pr['number']
		self.title = pr["title"]
		self.state = pr["state"]
		self.mergeable = pr["merge_commit_sha"] ##FIXME will become mergeable in the future 
		self.merged = pr["merged_at"]
		self.sha= pr["head"]["sha"]
		if self.state == 'open':
			self.origin=pr["head"]["repo"]["full_name"]

		if github != None:
			resource='issues/' + str(self.number) + "/labels"
			r = github._request(resource,{})
			self.labels=[]
			for l in r.json():
				#print "DEBUG label is ",l
				self.labels.append( l["name"]) 
	def Print(self):
		print "PR:",self.number,self.title
		print "    state:",self.state
		print "    sha:", self.sha
		print "    origin:", self.origin
		print "    mergeable:",self.mergeable
		print "    merged at:",self.merged
		print "    labels:",','.join(self.labels)
		pass

class Tag:
	''' Handle Tag '''
	def __init__(self):
		self.sha=""
		self.name=""

	def ParseRequest(self,tag):
		self.name=tag["name"]
		self.sha=tag["commit"]["sha"]
		return self

class Commit:
	''' Handle Commit '''
	def __init__(self):
		self.sha=""
		self.date=None
	def ParseRequest(self,cm):
		self.sha=cm["sha"]
		#self.date=cm["commit"]["committer"]["date"] ## or author ?
		#2015-10-29T14:50:49Z
		#self.date= datetime.strptime( cm["commit"]["committer"]["date"].replace('T',' ').replace('Z','') , "%Y-%m-%d %H:%M:%S" )## or author ?
		self.date= dateutil.parser.parse( cm["commit"]["committer"]["date"])
		return self
class Status:
	''' Handle Statuses '''
	def __init__(self):
		self.sha =""
		self.state=""
		self.context=""
		self.targeturl=""
		self.creator = ""

	def ParseRequest(self,st,sha):
		self.sha = sha
		self.state = st["state"]
		self.context= st["context"]
		self.targeturl= st["target_url"]
		self.creator = st["creator"]["login"]

#######################################################################################
#######                           GITHUB HANDLER                               ########
#######################################################################################

class  GitHubHandler:
	''' Handle Interface with github api'''
	def __init__(self,owner, repo ):
		self.owner = owner
		self.repo = repo
		self.baseurl='https://api.github.com/repos'
		self.pulls=[]
		self.tags=[]
		self.commits={} ## sha -> commit
		self.statuses={} ## sha, context  -> state
		self.milestones=[]
		self.head="" ## sha of the head

		# for post -- or auth req
		self.auth="" ## auth token

		# counters
		self.nleft = 5000

	def _count(self,req):
		limit = int(req.headers["X-RateLimit-Limit" ] )
		self.nleft = int(req.headers["X-RateLimit-Remaining"])
		reset = int(req.headers["X-RateLimit-Reset"])
		if self.nleft < 100:
			print " LIMIT of Request per hour is ", limit 
			print " LEFT is ",self.nleft, 
			print " RESET is ",reset
			now = datetime.now()	
			reset_time = datetime.fromtimestamp(reset)
			print " RESET will be in " + str( reset_time - now )
		if self.nleft < 10 :
			raise Exception("Too few Request left")
		return self

	def _request(self, resource, params):
		authstring = ""
		if self.auth != "": authstring='?access_token=%s'%(self.auth)
		url = '/'.join([self.baseurl,self.owner,self.repo, resource])
		url += authstring
		#print "DEBUG:  Handling REQ: ",url,params
		headers={"Accept":"application/vnd.github.v3+json"}
		r = requests.get(url,params = params,headers=headers)
		#print "DEBUG: Response is",r

		self._count(r)

		if r.status_code == 403:
			print "Request Failed:"
			print "   URL= '"+ url+ "'"
			print "   PARAMS= '"+ str(params) + "'"
			print "   HEADER= '"+ str(headers) + "'"
			raise Exception("403 Response")
		if r.status_code == 404:
			print "Request Failed:"
			print "   URL= '"+ url+ "'"
			print "   PARAMS= '"+ str(params) + "'"
			print "   HEADER= '"+ str(headers) + "'"
			raise Exception("404 Response")

		return r
	def _post(self, resource, params):
		authstring='?access_token=%s'%(self.auth)
		url = '/'.join([self.baseurl,self.owner,self.repo,resource])
		url+= authstring
		data=json.dumps(params)
		r = requests.post(url,data=data)
		self._count(r)
		return r

	def get_pullreq(self,state='open'):
		'''state can be open/closed'''
		self.pulls=[] ## reset

		resource = 'pulls'
		params = { "state": state }
		r  = self._request(resource,params)

		## parse the request to a proper format
		for pr in r.json():
			myPR = PullReq()
			myPR.ParseRequest(pr,self) ## self is passed to allow the Parsing to made additional calls to the issues, for labels. if unwanted pass None
			self.pulls.append(myPR)

		return self

	def get_tags(self):
		self.tags=[]

		resource='tags'
		params={}
		r = self._request(resource,params)

		for tag in r.json():
			#print "TAG = ",tag
			myTag= Tag()
			myTag.ParseRequest(tag)
			self.tags.append(myTag)

		return self

	def get_milestones(self,state='open'):
		self.milestones=[]
		resource = 'milestones'
		params={'state':state}
		r = self._request(resource,params)
		## TODO

	def get_head(self):
		#"https://api.github.com/repos/MiT-HEP/NeroProducer/git/refs/heads/master"
		r = self._request("git/refs/heads/master",{})
		cm = r.json()
		self.head=cm["object"]["sha"]
		return self

	def get_commits(self):
		## it returns 30 commits from a sha entry points starting from head
		self.get_head()
		#r = self._request("commits/"+self.head , {} ) 
		r = self._request("commits" , {} ) 
		for cm in r.json():
			myCommit = Commit()
			myCommit.ParseRequest(cm)
			self.commits[myCommit.sha] = myCommit
		return self

	def get_statuses(self, sha ) :
		r = self._request("statuses/" + sha ,{} )
		#self.statuses={}
		for key in r.json():
			st = Status()
			st.ParseRequest(key,sha) 
			self.statuses[ (st.sha, st.context) ] = st
		return self


	def set_status(self, sha, state="success", context="build", targeturl="url"):
		## check if it exists
		self.get_statuses(sha)
		if (sha,context) in self.statuses: 
			print "* Status",context,"already set for ",sha
			return self
		payload={"state":state,"context":context,"target_url":targeturl,"description":context}
		r = self._post("statuses/" + sha,payload)
		return self

	def read_token_fromfile(self,name):
		f = open(name)
		for l in f:
			l.split("#")[0]
			l = l.replace("\n",'')
			if l != "":
				self.auth=l[:]
				break;
		f.close()
###
