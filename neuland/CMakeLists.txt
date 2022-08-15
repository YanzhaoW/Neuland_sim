set(OtherLibs ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3BROOT_LIBRARIES} ${FAIRSOFT_LIBRARIES})
set(LIBRARY_NAME neuland)

file(GLOB SRCS
    *.cxx
    )
change_file_extension(*.cxx *.h HEADERS "${SRCS}")

add_library(${LIBRARY_NAME} SHARED ${SRCS} ${HEADERS} )
set_target_properties(${LIBRARY_NAME} PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(${LIBRARY_NAME} PUBLIC ${OtherLibs})



