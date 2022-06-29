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


set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/modules" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/modules_old" ${CMAKE_MODULE_PATH})
set(CMAKE_MODULE_PATH "${SIMPATH}/lib/VGM-4.5.0/Modules" ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH ${SIMPATH} ${CMAKE_PREFIX_PATH})
set(CMAKE_MODULE_PATH "${R3BRoot}/cmake/modules" ${CMAKE_MODULE_PATH})


find_package(ROOT REQUIRED PATHS ${SIMPATH} )

# find_package(CLHEP MODULE REQUIRED PATHS ${SIMPATH}/lib/VGM-4.5.0/Modules)
find_package(CLHEP MODULE REQUIRED)

# Now set them to FairRoot_LIBRARIES
set(FAIRROOT_LIBRARY_DIR "${FAIRROOTPATH}/lib")
set(FAIRROOT_LIBRARIES)
if(MSVC)
  set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
endif()
foreach(_cpt ${FairRootlibs})
    find_library(FAIRROOT_${_cpt}_LIBRARY ${_cpt} HINTS ${FAIRROOT_LIBRARY_DIR})
    if(FAIRROOT_${_cpt}_LIBRARY)
        mark_as_advanced(FAIRROOT_${_cpt}_LIBRARY)
        list(APPEND FAIRROOT_LIBRARIES ${FAIRROOT_${_cpt}_LIBRARY})
    endif()
endforeach()
if(FAIRROOT_LIBRARIES)
  list(REMOVE_DUPLICATES FAIRROOT_LIBRARIES)
endif()


# Now set them to FairRoot_LIBRARIES
set(FAIRSOFT_LIBRARY_DIR "${SIMPATH}/lib")
set(FAIRSOFT_LIBRARIES)
if(MSVC)
  set(CMAKE_FIND_LIBRARY_PREFIXES "lib")
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
endif()
foreach(_cpt ${FairSoftlibs})
    find_library(FAIRSOFT_${_cpt}_LIBRARY ${_cpt} HINTS ${FAIRSOFT_LIBRARY_DIR})
    message(STATUS "library: ${FAIRSOFT_${_cpt}_LIBRARY}")
    if(FAIRSOFT_${_cpt}_LIBRARY)
        mark_as_advanced(FAIRSOFT_${_cpt}_LIBRARY)
        list(APPEND FAIRSOFT_LIBRARIES ${FAIRSOFT_${_cpt}_LIBRARY})
    endif()
endforeach()
if(FAIRSOFT_LIBRARIES)
  list(REMOVE_DUPLICATES FAIRSOFT_LIBRARIES)
endif()

list(APPEND FAIRSOFT_LIBRARIES ${CLHEP_LIBRARIES})









# find_package(FairCMakeModules 0.1)
# if(FairCMakeModules_FOUND)
#   include(FairFindPackage2)
# else()
#   message(STATUS "Could not find FairCMakeModules. ")
# endif()

# if(COMMAND find_package2)
#   find_package2(PUBLIC FairRoot)
# else()
#   find_package(FairRoot)
# endif()

# include(FairMacros)
# include(CheckCompiler)

# find_package2(PUBLIC ROOT  VERSION 6.10.00  REQUIRED PATHS ${SIMPATH} )
# check_compiler()

# if(DEFINED $ENV{WERROR} AND $ENV{WERROR})
#   message(STATUS "Will compile with -Werror. ")
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-error=sign-compare -Wno-error=reorder -Wno-error=unused-variable -Wno-error=unused-but-set-variable")
# else()
#   if (NOT APPLE)
#     message(STATUS "Set env WERROR to 1 to enable -Werror. If origin/dev compiles on your platform with that option, it is definitly a good idea to do that.")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-sign-compare -Wno-reorder -Wno-unused-variable -Wno-unused-but-set-variable")
#   endif()
# endif()

# SetBasicVariables()
