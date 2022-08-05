set(OtherLibs ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3BROOT_LIBRARIES} ${FAIRSOFT_LIBRARIES})
set(LIBRARY_NAME share)

file(GLOB SRCS
    *.cxx
    )
change_file_extension(*.cxx *.h HEADERS "${SRCS}")

# root_generate_dictionary("G__Dict${LIBRARY_NAME}" "${HEADERS}" LINKDEF ${LINKDEF})
# add_library(digitizing ${SRCS})
# target_link_libraries(digitizing PUBLIC ${OtherLibs})

add_library(${LIBRARY_NAME} SHARED ${SRCS} ${HEADERS} )
set_target_properties(${LIBRARY_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${LIBRARY_NAME} PUBLIC ${OtherLibs})
