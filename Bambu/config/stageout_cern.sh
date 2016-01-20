# set this
USER=

for I in {0..9}
do
  lcg-cp -b -D srmv2 file://$PWD/nero.root srm://srm-eoscms.cern.ch:8443/srm/v2/server?SFN=/eos/cms/store/caf/user/$USER/nerov5/$DATASET/nero_${FILESET}.root && break
done
[ $I -lt 9 ]
