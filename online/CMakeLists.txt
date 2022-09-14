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
set(LIBRARY_NAME online)
set(LINKDEF NeulandOnlineLinkDef.h)
set(OtherLibs ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3BROOT_LIBRARIES} ${FAIRSOFT_LIBRARIES})

set(SRCS
    R3BNeulandOnlineReconstruction.cxx
    R3BNeulandOnlineSpectra.cxx
    )
change_file_extension(*.cxx *.h HEADERS "${SRCS}")

root_generate_dictionary("G__Dict${LIBRARY_NAME}" "${HEADERS}" LINKDEF ${LINKDEF})
add_library(${LIBRARY_NAME} SHARED ${SRCS} ${HEADERS} "G__Dict${LIBRARY_NAME}.cxx")
target_include_directories(${LIBRARY_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
set_target_properties(${LIBRARY_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${LIBRARY_NAME} PUBLIC ${OtherLibs})

list(APPEND NEULAND_LIBRARIES ${LIBRARY_NAME})
set(NEULAND_LIBRARIES ${NEULAND_LIBRARIES} PARENT_SCOPE)
