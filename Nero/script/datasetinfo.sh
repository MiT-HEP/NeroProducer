#!/bin/bash

DATASET=$1
echo "DATASET=$DATASET"

RUNRANGE=$(das_client.py --query "run dataset=$DATASET" --limit=200 | sort -n |grep -v "Showing" | grep -v '^$' | sed -ne '1p;$p' | tr '\n' ',')
EVENTS=$(das_client.py --query "summary dataset=$DATASET" | grep 'nevents\|nlumis' | cut -d':' -f 2 | tr '\n' ',')

echo "#runrange -- events, lumi"
echo "$RUNRANGE  - $EVENTS"

