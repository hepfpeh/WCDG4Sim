#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
include(${Geant4_USE_FILE})


# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS RIO Net)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
#include(${ROOT_USE_FILE})
#include($ENV{ROOTSYS}/share/root/cmake/modules/RootNewMacros.cmake)

#----------------------------------------------------------------------------
#  Find boost libraries

#find_package(boost REQUIRED)


#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
		    ${PROJECT_SOURCE_DIR}/cla/include
                    ${ROOT_INCLUDE_DIRS}
		    ${Geant4_INCLUDE_DIR}
		   )
file(GLOB e_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
file(GLOB e_headers ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#

add_executable(ElecSim electronics.cc ${e_sources} ${e_headers})
target_link_libraries(ElecSim cla ${ROOT_LIBRARIES} )

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build RE02. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(elec_SCRIPTS
    pulses.C
    pulses2.C
    pulses3.C
  )

foreach(_script ${elec_SCRIPTS})
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/elec/${_script}
    COPYONLY
    )
endforeach()


