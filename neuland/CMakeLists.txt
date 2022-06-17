set(OtherLibs ROOT::Core R3BData VMCLibrary Physics R3Bbase R3BNeulandShared)

add_library(neuland
    R3BNeuland.cxx)

target_link_libraries(neuland PUBLIC ${OtherLibs})

