# source /u/land/fake_cvmfs/sourceme.sh

export SIMPATH=/u/land/fake_cvmfs/9.13/FairSoft/deps
export FAIRROOTPATH=/u/land/fake_cvmfs/9.13/FairRoot/install-18.2.1
export R3BROOTPATH=/u/yanwang/software/R3BRoot
export ROOT_INCLUDE_PATH=/u/land/fake_cvmfs/9.13/FairSoft/deps/include/root6:
export VMCWORKDIR=/u/yanwang/software/src/R3BRoot
export UCESB_DIR=/u/land/fake_cvmfs/9.13/ucesb



#ROOT config:
source ${SIMPATH}/bin/thisroot.sh
#geant4 config
pushd ${SIMPATH}/bin
. ${SIMPATH}/bin/geant4.sh
popd >/dev/null

