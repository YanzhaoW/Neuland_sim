# source /u/land/fake_cvmfs/sourceme.sh

export SIMPATH=/u/land/fake_cvmfs/9.13/FairSoft/install-nov22-dev
export FAIRROOTPATH=/u/land/fake_cvmfs/9.13/FairSoft/install-nov22-dev
export R3BROOTPATH=/u/yanwang/software/r3blib
export ROOT_INCLUDE_PATH=/u/land/fake_cvmfs/9.13/FairSoft/install-nov22-dev/include/TGeant3:/u/land/fake_cvmfs/9.13/FairSoft/install-nov22-dev/include
export VMCWORKDIR=/u/yanwang/git_forks/R3BRoot
export UCESB_DIR=/u/land/fake_cvmfs/9.13/ucesb



#ROOT config:
source ${SIMPATH}/bin/thisroot.sh
#geant4 config
pushd ${SIMPATH}/bin
. ${SIMPATH}/bin/geant4.sh
popd >/dev/null

