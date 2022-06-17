SET(SIMPATH $ENV{SIMPATH})
SET(FAIRROOTPATH $ENV{FAIRROOTPATH})

set(CMAKE_MODULE_PATH "${FAIRROOTPATH}/share/fairbase/cmake/modules" ${CMAKE_MODULE_PATH})
set(CMAKE_PREFIX_PATH "${SIMPATH}/share/cmake/FairCMakeModules-0.2.0" ${CMAKE_PREFIX_PATH})
set(CMAKE_PREFIX_PATH ${SIMPATH} ${CMAKE_PREFIX_PATH})
set(CMAKE_MODULE_PATH "${R3BRoot}/cmake/modules" ${CMAKE_MODULE_PATH})

find_package(FairCMakeModules 0.1)
if(FairCMakeModules_FOUND)
  include(FairFindPackage2)
else()
  message(STATUS "Could not find FairCMakeModules. ")
endif()

if(COMMAND find_package2)
  find_package2(PUBLIC FairRoot)
else()
  find_package(FairRoot)
endif()

include(FairMacros)
include(CheckCompiler)

# find_package2(PUBLIC ROOT  VERSION 6.10.00  REQUIRED PATHS ${SIMPATH} )
find_package(ROOT 6.24.02 EXACT REQUIRED PATHS ${SIMPATH} )
check_compiler()

if(DEFINED $ENV{WERROR} AND $ENV{WERROR})
  message(STATUS "Will compile with -Werror. ")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wno-error=sign-compare -Wno-error=reorder -Wno-error=unused-variable -Wno-error=unused-but-set-variable")
else()
  if (NOT APPLE)
    message(STATUS "Set env WERROR to 1 to enable -Werror. If origin/dev compiles on your platform with that option, it is definitly a good idea to do that.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-sign-compare -Wno-reorder -Wno-unused-variable -Wno-unused-but-set-variable")
  endif()
endif()

SetBasicVariables()
