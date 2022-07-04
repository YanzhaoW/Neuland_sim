cmake_minimum_required(VERSION 3.13.4)

project(testing)



# set(R3Blibs R3BPassive R3Bbase R3BNeulandSimulation R3BData R3BGen)
set(R3Blibs R3BPassive R3BBase R3BNeulandSimulation R3BData R3BGen R3BNeulandShared Field R3BTracking)
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
    "${Geant4_INCLUDE_DIRS}" 
    "${ROOT_INCLUDE_DIRS}"
    "${R3BROOT}/include" 
    # "${R3BRoot}/r3bgen"
    )

add_executable(simulate simulate.cxx)
target_link_libraries(simulate digitizing ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3BROOT_LIBRARIES} ${FAIRSOFT_LIBRARIES})


add_subdirectory(digitizing)

add_subdirectory(Protobuf)
add_executable(checkout checkout.cxx)
# # target_link_libraries(checkout ROOT::Core digitizing Proto ${R3Blibs} ${FairRootlibs} ${FairSoftlibs})
target_link_libraries(checkout digitizing Proto ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3BROOT_LIBRARIES} ${FAIRSOFT_LIBRARIES})
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
