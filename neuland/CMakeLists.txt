set(OtherLibs ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3BROOT_LIBRARIES} ${FAIRSOFT_LIBRARIES})

add_library(neuland
    R3BNeuland.cxx)

target_link_libraries(neuland PUBLIC ${OtherLibs})


