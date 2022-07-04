cmake_minimum_required(VERSION 3.13.4)

project(testing)

set(R3Blibs R3BPassive R3BBase R3BNeulandSimulation R3BData R3BGen R3BNeulandShared Field R3BTracking)
set(FairRootlibs FairTools Base Gen TrkBase ParBase GeoBase FairLogger EventDisplay )
Set(FairSoftlibs G4geometry G4physicslists geant4vmc G4run G4event)

include(${CMAKE_SOURCE_DIR}/cmake_additional.cmake)

include_directories(
    "${FAIRROOTPATH}/include"
    "${CMAKE_SOURCE_DIR}/neuland"
    "${CMAKE_SOURCE_DIR}/digitizing"
    "${SIMPATH}/include" 
    # "${SIMPATH}/include/vmc" 
    "${SIMPATH}/include/geant4vmc" 
    "${SIMPATH}/include/Geant4" 
    "${SIMPATH}/include/root6"
    "${R3BROOT}/include" 
    # "${R3BRoot}/r3bgen"
    )

add_executable(simulate simulate.cxx)
target_link_libraries(simulate digitizing ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3BROOT_LIBRARIES} ${FAIRSOFT_LIBRARIES})


add_subdirectory(digitizing)

add_subdirectory(Protobuf)
add_executable(checkout checkout.cxx)
target_link_libraries(checkout digitizing Proto ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3BROOT_LIBRARIES} ${FAIRSOFT_LIBRARIES})

install(
    FILES ${CMAKE_BINARY_DIR}/compile_commands.json
    DESTINATION ${CMAKE_SOURCE_DIR}
    )
