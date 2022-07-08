cmake_minimum_required(VERSION 3.13.4)

project(testing)

set(R3Blibs R3BPassive R3BBase R3BNeulandSimulation R3BData R3BGen R3BNeulandShared Field R3BTracking R3Bsource R3BNeulandCalibration R3BLos R3BPsp R3BTCal)
set(FairRootlibs FairTools Base Gen TrkBase ParBase GeoBase FairLogger EventDisplay )
Set(FairSoftlibs geant4vmc)

include(${CMAKE_SOURCE_DIR}/cmake_modules/cmake_additional.cmake)

include_directories(
    "${FAIRROOTPATH}/include"
    "${CMAKE_SOURCE_DIR}/neuland"
    "${CMAKE_SOURCE_DIR}/digitizing"
    "${SIMPATH}/include" 
    "${SIMPATH}/include/geant4vmc" 
    "${SIMPATH}/include/Geant4" 
    "${SIMPATH}/include/root6"
    "${R3BROOT}/include" 
    "${ucesb_INCLUDE_DIR}"
    )

# making simulation executable:
add_executable(simulate simulate.cxx)
target_link_libraries(simulate digitizing ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3BROOT_LIBRARIES} ${FAIRSOFT_LIBRARIES})

# make digitization executable:
add_subdirectory(digitizing)
add_subdirectory(Protobuf)
add_executable(checkout checkout.cxx)
target_link_libraries(checkout digitizing Proto ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3BROOT_LIBRARIES} ${FAIRSOFT_LIBRARIES})
target_compile_options(checkout PRIVATE -Wno-shadow)

# making experimental data reader executable:
add_executable(reader reader.cxx)
target_link_libraries(reader ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3BROOT_LIBRARIES} ${FAIRSOFT_LIBRARIES})



# add_executable(plot plot.cxx)
# target_link_libraries(plot ROOT::RIO ROOT::Core ROOT::MathCore Gpad Hist Graf)
# message(STATUS ${ROOT_LIBRARIES})


install(
    FILES ${CMAKE_BINARY_DIR}/compile_commands.json
    DESTINATION ${CMAKE_SOURCE_DIR}
    )
