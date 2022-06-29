set(OtherLibs ${ROOT_LIBRARIES} R3BData VMCLibrary Physics R3Bbase R3BNeulandShared Hist Field)

add_library(digitizing
    DigitizingEngine.cxx
    DigitizingTacQuila.cxx
    R3BNeulandDigitizer.cxx
    R3BNeulandHitMon.cxx
    DigitizingTamex.cxx
    R3BNeulandField.cxx
    )

target_link_libraries(digitizing PUBLIC ${OtherLibs})

