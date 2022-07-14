##############################################################################
#   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    #
#   Copyright (C) 2019 Members of R3B Collaboration                          #
#                                                                            #
#             This software is distributed under the terms of the            #
#                 GNU General Public Licence (GPL) version 3,                #
#                    copied verbatim in the file "LICENSE".                  #
#                                                                            #
# In applying this license GSI does not waive the privileges and immunities  #
# granted to it by virtue of its status as an Intergovernmental Organization #
# or submit itself to any jurisdiction.                                      #
##############################################################################

set(LIBRARY_NAME R3BCalibration)
set(LINKDEF NeulandCalibrationLinkDef.h)
set(OtherLibs ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3BROOT_LIBRARIES} ${FAIRSOFT_LIBRARIES})

set(SRCS
    LSQR.cxx
    R3BNeulandCal2Hit.cxx
    R3BNeulandCal2HitPar.cxx
    R3BNeulandCosmicTracker.cxx
    R3BNeulandHitCalibrationBar.cxx
    R3BNeulandHitCalibrationEngine.cxx
    R3BNeulandHitHist.cxx
    R3BNeulandHitModulePar.cxx
    R3BNeulandHitPar.cxx
    R3BNeulandMapped2Cal.cxx
    R3BNeulandMapped2CalPar.cxx
    R3BNeulandMappedHist.cxx
    R3BNeulandParFact.cxx
    R3BNeulandProvideTStart.cxx
    R3BNeulandQCalFiller.cxx
    R3BNeulandQCalPar.cxx
    R3BNeulandTSyncer.cxx
    )
change_file_extension(*.cxx *.h HEADERS "${SRCS}")

root_generate_dictionary("G__Dict${LIBRARY_NAME}" "${HEADERS}" LINKDEF ${LINKDEF})
add_library(${LIBRARY_NAME} SHARED ${SRCS} ${HEADERS} "G__Dict${LIBRARY_NAME}.cxx")
target_include_directories(${LIBRARY_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
set_target_properties(${LIBRARY_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${LIBRARY_NAME} PUBLIC ${OtherLibs})
