cmake_minimum_required(VERSION 3.13.4)

project(testing)

# set(R3Blibs R3BPassive R3BBase R3BNeulandSimulation R3BData R3BGen R3BNeulandShared Field R3BTracking R3Bsource R3BLos R3BPsp R3BTCal)
set(R3Blibs R3BPassive R3BBase R3BData R3BGen R3BNeulandShared Field R3BTracking R3Bsource R3BLos R3BPsp R3BTCal R3BNeulandDigitizing R3BNeulandSimulation R3BNeulandCalibration)
set(FairRootlibs FairTools Base Gen TrkBase ParBase GeoBase FairLogger EventDisplay )
Set(FairSoftlibs geant4vmc)

include(${CMAKE_SOURCE_DIR}/cmake_modules/cmake_additional.cmake)

include_directories(
    "${CMAKE_SOURCE_DIR}/share"
    "${CMAKE_SOURCE_DIR}/r3bneuland"
    # "${CMAKE_SOURCE_DIR}/digitizing"
    "${CMAKE_SOURCE_DIR}/online"
    "${CMAKE_SOURCE_DIR}/calibration"
    # "${CMAKE_SOURCE_DIR}/R3BCalibration"
    "${CMAKE_SOURCE_DIR}/test"
    "${R3BROOT}/include" 
    "${FAIRROOTPATH}/include"
    "${SIMPATH}/include" 
    "${SIMPATH}/include/geant4vmc" 
    "${SIMPATH}/include/Geant4" 
    "${SIMPATH}/include/root6"
    "${ucesb_INCLUDE_DIR}"
    )

# make digitization executable:
set(NEULAND_LIBRARIES)
add_subdirectory(r3bneuland)
# add_subdirectory(digitizing)
add_subdirectory(calibration)
add_subdirectory(Protobuf)
# add_subdirectory(R3BCalibration)
add_subdirectory(online)
add_subdirectory(test)
add_subdirectory(share)



# # making simulation executable:
add_executable(neuland neuland.cxx)
target_link_libraries(neuland r3bneuland share Boost::program_options)

# add_executable(simulate simulate.cxx)
# target_link_libraries(simulate r3bneuland share online calibration digitizing ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3BROOT_LIBRARIES} ${FAIRSOFT_LIBRARIES})


# add_executable(checkout checkout.cxx)
# # target_link_libraries(checkout calibration digitizing Proto ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3BROOT_LIBRARIES} ${FAIRSOFT_LIBRARIES})
# target_link_libraries(checkout r3bneuland share online calibration digitizing ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3BROOT_LIBRARIES} ${FAIRSOFT_LIBRARIES})
# target_compile_options(checkout PRIVATE -Wno-shadow)

# making experimental data reader executable:
add_executable(reader reader.cxx)
target_link_libraries(reader r3bneuland  ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3BROOT_LIBRARIES} ${FAIRSOFT_LIBRARIES})

# add_executable(plot plot.cxx)
# target_link_libraries(plot ROOT::RIO ROOT::Core ROOT::MathCore Gpad Hist Graf)
# message(STATUS ${ROOT_LIBRARIES})

install(
    FILES ${CMAKE_BINARY_DIR}/compile_commands.json
    DESTINATION ${CMAKE_SOURCE_DIR}
    )
