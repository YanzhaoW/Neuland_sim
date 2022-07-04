# set(Protobuf_PREFIX_PATH
#     "${HOME}/software/src/protobuf-3.20.1/" )
# list(APPEND CMAKE_PREFIX_PATH "${Protobuf_PREFIX_PATH}")

set( Protobuf_INCLUDE_DIR "${SIMPATH}/include")
set(OtherLibs ${ROOT_LIBRARIES} ${FAIRROOT_LIBRARIES} ${R3BROOT_LIBRARIES} ${FAIRSOFT_LIBRARIES})
# find_package(Protobuf REQUIRED PATH "${HOME}/software/src/protobuf-3.20.1/cmake")
find_package(Protobuf REQUIRED)


if(${Protobuf_FOUND})
    message(STATUS "Protobuf found. Version: ${Protobuf_VERSION}")
else()
    message(STATUS "Protobuf not found.")
endif()

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS NeulandHit.proto)
# SET_SOURCE_FILES_PROPERTIES(${PROTO_SRC} ${PROTO_HDRS} PROPERTIES GENERATED TRUE)
add_library(Proto ${PROTO_SRCS} ${PROTO_HDRS} R3BNeulandHitProto.cxx)
target_link_libraries(Proto PUBLIC ${OtherLibs} ${PROTOBUF_LIBRARY})
target_include_directories(Proto PUBLIC ${Protobuf_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
target_compile_options(Proto PRIVATE -Wno-shadow)
# message(STATUS "properties: ${PROTOBUF_LIBRARY}")
