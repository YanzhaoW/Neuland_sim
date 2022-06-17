cmake_minimum_required(VERSION 3.20.1)

project(testing)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(ENV{FAIRROOTPATH} "/data/ywang/software/FairRoot")
set(ENV{SIMPATH} "/data/ywang/software/FairSoft")
SET(R3BRoot "/data/ywang/software/R3BRoot")
SET(R3BINSTALL "/data/ywang/software/r3blib")


include(${CMAKE_SOURCE_DIR}/cmake_additional.cmake)

include_directories(
    "${CMAKE_SOURCE_DIR}/neuland"
    "${CMAKE_SOURCE_DIR}/digitizing"
    "${SIMPATH}/include" 
    "${SIMPATH}/include/vmc" 
    "${SIMPATH}/include/geant4vmc" 
    "${SIMPATH}/include/Geant4" 
    "${SIMPATH}/include/root"
    "${R3BINSTALL}/include" 
    "${R3BRoot}/r3bgen"
    )

link_directories(${SIMPATH}/lib ${FAIRROOT_LIBRARY_DIR} "${R3BINSTALL}/lib")

# set(CMAKE_SKIP_BUILD_RPATH FALSE)
# set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
# SET(CMAKE_BUILD_RPATH "${SIMPATH}/lib")
# set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(R3Blibs R3BPassive R3Bbase R3BNeulandSimulation R3BData R3BGen)
# set(R3Blibs R3BPassive R3BNeulandSimulation)
set(FairRootlibs FairTools Base Gen TrkBase ParBase GeoBase FairLogger EventDisplay )
Set(FairSoftlibs Geom Physics geant4vmc G4run G4event CLHEP)

# SET(CMAKE_EXE_LINKER_FLAGS 
#           "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath -Wl,${SIMPATH}/include/root")


# find_library(CORE NAMES Core PATHS "${SIMPATH}/lib")
add_executable(simulate simulate.cxx)
add_subdirectory(digitizing)
# target_link_libraries(${PROJECT_NAME} ROOT::Core ${FairRootlibs} ${R3Blibs} ${FairSoftlibs} neuland)
target_link_libraries(simulate digitizing ROOT::Core ROOT::RIO ${FairRootlibs} ${R3Blibs} ${FairSoftlibs})



add_subdirectory(Protobuf)
add_executable(checkout checkout.cxx)
# target_link_libraries(checkout ROOT::Core digitizing Proto ${R3Blibs} ${FairRootlibs} ${FairSoftlibs})
target_link_libraries(checkout digitizing Proto ${R3Blibs} ${FairRootlibs} ${FairSoftlibs})
target_compile_options(checkout PRIVATE -Wno-shadow)
# get_property(proper TARGET checkout PROPERTY BUILD_RPATH)


# add_executable(plot plot.cxx)
# target_link_libraries(plot ROOT::RIO ROOT::Core ROOT::MathCore Gpad Hist Graf)
# message(STATUS ${ROOT_LIBRARIES})


install(
    FILES ${CMAKE_BINARY_DIR}/compile_commands.json
    DESTINATION ${CMAKE_SOURCE_DIR}
    )
