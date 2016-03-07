#!/bin/bash

QUEUE=8nh

### check that are all ok and resubmit in case they are not
function check_and_resubmit {
	ALLOK=0
	echo "<-> Checking dirs: $@"
	for dir in $@ 
	do
		python sendOnBatch.py --status -d $dir 2>&1 >/dev/null
		exit=$? 
		echo "<-> Status for $dir is $exit <->";
		[ $exit -eq 0 ]	|| python sendOnBatch.py --only-submit -j fail -d $dir -q $QUEUE 2>&1 >/dev/null
		[ $exit -eq 0 ] || { ALLOK=1 ; }
	done
	return ALLOK
}

function batchjobs {
	N=$(bjobs | grep -v 'JOBID' | wc -l)
	echo "<-> $N jobs on the batch queues"
	[ $N -gt 0 ] && return 0
	return 1
}

function mailme {
	SUBJECT=$1
	BODY=$2
	echo $BODY | mail -s $SUBJECT $USER@cern.ch
	return 0
}


## loop main

DIRS=$@
#echo "DIRS=$DIRS"
while sleep 1m; 
do
	echo ".. wake up"
	[ "X$DIRS" != "X" ] && check_and_resubmit $DIRS  && mailme Done "$DIRS" && exit 0  ## if successfull gracely exit
	batchjobs || { mailme Fail "No job left" && exit 0 ; } ## tell me that failed
	echo -n "sleep "
done
