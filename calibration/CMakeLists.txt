set(OtherLibs ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3BROOT_LIBRARIES} ${FAIRSOFT_LIBRARIES})

add_library(calibration
    NeulandCalTesting.cxx
    NeulandCalTesting.h
    )

target_link_libraries(calibration PUBLIC ${OtherLibs})

