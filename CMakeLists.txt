cmake_minimum_required(VERSION 3.13.4)

project(testing)


# IF(NOT DEFINED ENV{SIMPATH})
#   MESSAGE(FATAL_ERROR "You did not define the environment variable SIMPATH which is nedded to find the external packages. Please set this variable and execute cmake again.")
# ENDIF(NOT DEFINED ENV{SIMPATH})
# SET(SIMPATH $ENV{SIMPATH})

# SET(R3BRoot "/data/ywang/software/R3BRoot")
# SET(R3BINSTALL "/data/ywang/software/r3blib")

set(R3Blibs R3BPassive R3Bbase R3BNeulandSimulation R3BData R3BGen)
set(FairRootlibs FairTools Base Gen TrkBase ParBase GeoBase FairLogger EventDisplay )
Set(FairSoftlibs Geom Physics geant4vmc G4run G4event)

include(${CMAKE_SOURCE_DIR}/cmake_additional.cmake)

include_directories(
    "${FAIRROOTPATH}/include"
    "${CMAKE_SOURCE_DIR}/neuland"
    "${CMAKE_SOURCE_DIR}/digitizing"
    "${SIMPATH}/include" 
    # "${SIMPATH}/include/vmc" 
    "${SIMPATH}/include/geant4vmc" 
    "${SIMPATH}/include/vmc" 
    "${SIMPATH}/include/Geant4" 
    "${SIMPATH}/include/root6"
    "${R3BROOT}/include" 
    # "${R3BRoot}/r3bgen"
    )

# link_directories(${SIMPATH}/lib/root ${FAIRROOT_LIBRARY_DIR} "${R3BROOT}/lib")

# set(CMAKE_SKIP_BUILD_RPATH FALSE)
# set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
# SET(CMAKE_BUILD_RPATH "${SIMPATH}/lib")
# set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)



# Now set them to FairRoot_LIBRARIES
set(FAIRROOT_LIBRARY_DIR "${FAIRROOTPATH}/lib")
set(FAIRROOT_LIBRARIES)
if(MSVC)
  set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
endif()
foreach(_cpt ${FairRootlibs})
    find_library(FAIRROOT_${_cpt}_LIBRARY ${_cpt} HINTS ${FAIRROOT_LIBRARY_DIR})
    if(FAIRROOT_${_cpt}_LIBRARY)
        mark_as_advanced(FAIRROOT_${_cpt}_LIBRARY)
        list(APPEND FAIRROOT_LIBRARIES ${FAIRROOT_${_cpt}_LIBRARY})
    endif()
endforeach()
if(FAIRROOT_LIBRARIES)
  list(REMOVE_DUPLICATES FAIRROOT_LIBRARIES)
endif()







# SET(CMAKE_EXE_LINKER_FLAGS 
#           "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,${SIMPATH}/include/root")


# find_library(CORE NAMES Core PATHS "${SIMPATH}/lib")
add_executable(simulate simulate.cxx)
add_subdirectory(digitizing)
# target_link_libraries(${PROJECT_NAME} ROOT::Core ${FairRootlibs} ${R3Blibs} ${FairSoftlibs} neuland)
target_link_libraries(simulate digitizing ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3Blibs} ${FairSoftlibs})
# set_target_properties(simulate PROPERTIES IMPORTED_LOCATION ${SIMPATH}/lib/)



# add_subdirectory(Protobuf)
# add_executable(checkout checkout.cxx)
# # target_link_libraries(checkout ROOT::Core digitizing Proto ${R3Blibs} ${FairRootlibs} ${FairSoftlibs})
# target_link_libraries(checkout digitizing Proto ${R3Blibs} ${FairRootlibs} ${FairSoftlibs})
# target_compile_options(checkout PRIVATE -Wno-shadow)
# # get_property(proper TARGET checkout PROPERTY BUILD_RPATH)


# add_executable(plot plot.cxx)
# target_link_libraries(plot ROOT::RIO ROOT::Core ROOT::MathCore Gpad Hist Graf)
# message(STATUS ${ROOT_LIBRARIES})


install(
    FILES ${CMAKE_BINARY_DIR}/compile_commands.json
    DESTINATION ${CMAKE_SOURCE_DIR}
    )
