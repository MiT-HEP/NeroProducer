# Script and Utilities

## SendOnBatch
* Send production on the batch system ( lxplus )
* check status of the production (--status)
* resubmit jobs (jobId, fail run ...)

## Setup.sh
* used to collect the commands to set up the area prior compilation.

## Cutflow.py
* reproduce the monojet cutflow to keep the synchronization.

## TestUnit.py
* try to load the core library and to get all the objects booked.
  After that call a test unit on the cutflow for CH and cutflow.py for monojet.

## DumpTree.py
* dump tree into txt files for synchronization purposes

## GetXsec.py
* read an eos directory and print on screen the internal xsec, and the sum of weights

## lumi.py
* print a json file from the ntuples using the all information.

## validate.py
* script to run on ntuples at the begginning and produce data/mc comparisons.
