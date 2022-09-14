set(OtherLibs ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3BROOT_LIBRARIES} ${FAIRSOFT_LIBRARIES})
set(LIBRARY_NAME digitizing)
set(LINKDEF NeulandDigitizingLinkDef.h)

set(SRCS
DigitizingEngine.cxx
DigitizingTacQuila.cxx
DigitizingTamex.cxx
R3BNeulandHitMon.cxx
R3BNeulandDigitizer.cxx
)
change_file_extension(*.cxx *.h HEADERS "${SRCS}")

# root_generate_dictionary("G__Dict${LIBRARY_NAME}" "${HEADERS}" LINKDEF ${LINKDEF})
# add_library(digitizing ${SRCS})
# target_link_libraries(digitizing PUBLIC ${OtherLibs})

root_generate_dictionary("G__Dict${LIBRARY_NAME}" "${HEADERS}" LINKDEF ${LINKDEF})
add_library(${LIBRARY_NAME} SHARED ${SRCS} ${HEADERS} "G__Dict${LIBRARY_NAME}.cxx")
target_include_directories(${LIBRARY_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
set_target_properties(${LIBRARY_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${LIBRARY_NAME} PUBLIC ${OtherLibs})

list(APPEND NEULAND_LIBRARIES ${LIBRARY_NAME})
set(NEULAND_LIBRARIES ${NEULAND_LIBRARIES} PARENT_SCOPE)
