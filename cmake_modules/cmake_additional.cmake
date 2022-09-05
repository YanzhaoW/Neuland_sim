set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

IF(NOT DEFINED ENV{FAIRROOTPATH})
  MESSAGE(FATAL_ERROR "You did not define the environment variable FAIRROOTPATH which is needed to find FairRoot. Please set this variable and execute cmake again.")
ENDIF(NOT DEFINED ENV{FAIRROOTPATH})
SET(FAIRROOTPATH $ENV{FAIRROOTPATH})

IF(NOT DEFINED ENV{SIMPATH})
  MESSAGE(FATAL_ERROR "You did not define the environment variable SIMPATH which is nedded to find the external packages. Please set this variable and execute cmake again.")
ENDIF(NOT DEFINED ENV{SIMPATH})
SET(SIMPATH $ENV{SIMPATH})


IF(NOT DEFINED ENV{R3BROOTPATH})
  MESSAGE(FATAL_ERROR "You did not define the environment variable R3BROOTPATH which is nedded to find the external packages. Please set this variable and execute cmake again.")
ENDIF(NOT DEFINED ENV{R3BROOTPATH})
SET(R3BROOT $ENV{R3BROOTPATH})


set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/modules" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/modules_old" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${SIMPATH}/lib/VGM-4.5.0/Modules" ${CMAKE_MODULE_PATH})
# set(CMAKE_MODULE_PATH "${SIMPATH}/share/cmake-3.13/Modules" ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${SIMPATH} ${CMAKE_PREFIX_PATH})
set(CMAKE_MODULE_PATH "${R3BRoot}/cmake/modules" ${CMAKE_MODULE_PATH})

Set(CheckSrcDir "${FAIRROOTPATH}/share/fairbase/cmake/checks")


include(CheckCXX11Features)
include(FairMacros)

# set(ROOT_CONFIG_DEBUG true)
find_package(FairRoot REQUIRED)
find_package(ucesb REQUIRED)
find_package(ROOT REQUIRED COMPONENTS Geom PATHS ${SIMPATH} )
find_package(Geant4 REQUIRED PATHS ${SIMPATH}/lib)

find_package(yaml-cpp REQUIRED PATHS ${SIMPATH}/lib)

# message(STATUS "use file: ${ROOT_USE_FILE}")
include(${ROOT_USE_FILE})

find_package(Geant4VMC REQUIRED)
if(Geant4VMC_FOUND)
  Set(Geant4VMC_LIBRARY_DIR "${Geant4VMC_DIR}/${Geant4VMC_CMAKE_INSTALL_LIBDIR}")
  Set(Geant4VMC_SHARE_DIR "${Geant4VMC_DIR}/share")
  find_path(Geant4VMC_MACRO_DIR NAMES g4libs.C PATHS
    "${Geant4VMC_SHARE_DIR}/Geant4VMC-${Geant4VMC_VERSION}/examples/macro"
    NO_DEFAULT_PATH
  )
Endif()


set(GEANT4_LIBRARY_DIR "${SIMPATH}/lib")
set(Geant4_DIR "${SIMPATH}/lib/Geant4-10.5.1")
set(Geant4_INCLUDE_DIRS "${Geant4_INCLUDE_DIRS}")

find_package(CLHEP MODULE REQUIRED)

# Now set them to FairRoot_LIBRARIES
set(FAIRROOT_LIBRARY_DIR "${FAIRROOTPATH}/lib")
set(FAIRROOT_LIBRARIES)
if(MSVC)
  set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
endif()
foreach(_cpt ${FairRootlibs} ${Geant4_LIBRARIES})
    find_library(FAIRROOT_${_cpt}_LIBRARY ${_cpt} HINTS ${FAIRROOT_LIBRARY_DIR})
    # message(STATUS "FairSoft: ${FAIRROOT_${_cpt}_LIBRARY}")
    if(FAIRROOT_${_cpt}_LIBRARY)
        mark_as_advanced(FAIRROOT_${_cpt}_LIBRARY)
        list(APPEND FAIRROOT_LIBRARIES ${FAIRROOT_${_cpt}_LIBRARY})
    else()
        message(FATAL_ERROR "library ${_cpt} from ${FAIRROOT_LIBRARY_DIR} is not found!")
    endif()
endforeach()
if(FAIRROOT_LIBRARIES)
  list(REMOVE_DUPLICATES FAIRROOT_LIBRARIES)
endif()


# Now set them to FairSoft_LIBRARIES
set(FAIRSOFT_LIBRARY_DIR "${SIMPATH}/lib")
set(FAIRSOFT_LIBRARIES)
if(MSVC)
  set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
endif()
foreach(_cpt ${FairSoftlibs} ${YAML_CPP_LIBRARIES})
    find_library(FAIRSOFT_${_cpt}_LIBRARY ${_cpt} HINTS ${FAIRSOFT_LIBRARY_DIR})
    if(FAIRSOFT_${_cpt}_LIBRARY)
        message(STATUS "library ${FAIRSOFT_${_cpt}_LIBRARY} is found!")
        mark_as_advanced(FAIRSOFT_${_cpt}_LIBRARY)
        list(APPEND FAIRSOFT_LIBRARIES ${FAIRSOFT_${_cpt}_LIBRARY})
    endif()
endforeach()
if(FAIRSOFT_LIBRARIES)
  list(REMOVE_DUPLICATES FAIRSOFT_LIBRARIES)
endif()

list(APPEND FAIRSOFT_LIBRARIES ${CLHEP_LIBRARIES})

# Now set them to R3BRoot_Libraries
set(R3BROOT_LIBRARY_DIR "${R3BROOT}/lib")
set(R3BROOT_LIBRARIES)
if(MSVC)
  set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
endif()
foreach(_cpt ${R3Blibs})
    find_library(R3BROOT_${_cpt}_LIBRARY ${_cpt} HINTS ${R3BROOT_LIBRARY_DIR})
    if(R3BROOT_${_cpt}_LIBRARY)
        mark_as_advanced(R3BROOT_${_cpt}_LIBRARY)
        list(APPEND R3BROOT_LIBRARIES ${R3BROOT_${_cpt}_LIBRARY})
    else()
        message(FATAL_ERROR "library ${_cpt} from ${R3BROOT_LIBRARY_DIR} is not found!")
    endif()
endforeach()
if(R3BROOT_LIBRARIES)
  list(REMOVE_DUPLICATES R3BROOT_LIBRARIES)
endif()

find_library(UCESB_LIBRARY ${ucesb_LIBRARY_SHARED} HINTS ${ucesb_LIBRARY_DIR})
list(APPEND R3BROOT_LIBRARIES ${UCESB_LIBRARY})






include(CheckCompiler)

check_compiler()

# if(DEFINED $ENV{WERROR} AND $ENV{WERROR})
#   message(STATUS "Will compile with -Werror. ")
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-error=sign-compare -Wno-error=reorder -Wno-error=unused-variable -Wno-error=unused-but-set-variable")
# else()
#   if (NOT APPLE)
#     message(STATUS "Set env WERROR to 1 to enable -Werror. If origin/dev compiles on your platform with that option, it is definitly a good idea to do that.")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-sign-compare -Wno-reorder -Wno-unused-variable -Wno-unused-but-set-variable")
#   endif()
# endif()

SetBasicVariables()
message(STATUS "cmake flags: ${CMAKE_CXX_FLAGS}")


