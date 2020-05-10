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


#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
                    ${Geant4_INCLUDE_DIR}
                   )
file(GLOB W_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
file(GLOB W_headers ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#

add_executable(WCDtankSim WCDtank.cc ${W_sources} ${W_headers})
target_link_libraries(WCDtankSim ${Geant4_LIBRARIES} )

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build RE02. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(WCDtank_SCRIPTS
    vis.mac
    muon.mac
    muon-vert.mac
    muon-nonvert.mac
  )

foreach(_script ${WCDtank_SCRIPTS})
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/WCDtank/${_script}
    COPYONLY
    )
endforeach()
