set(OtherLibs ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3BROOT_LIBRARIES} ${FAIRSOFT_LIBRARIES})
set(LIBRARY_NAME calibration)
add_library(calibration
    NeulandCalTesting.cxx
    NeulandCalTesting.h
    )

target_link_libraries(${LIBRARY_NAME} PUBLIC ${OtherLibs})

list(APPEND NEULAND_LIBRARIES ${LIBRARY_NAME})
set(NEULAND_LIBRARIES ${NEULAND_LIBRARIES} PARENT_SCOPE)
