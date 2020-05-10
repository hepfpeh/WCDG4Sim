#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include
                   )

file(GLOB c_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
file(GLOB c_headers ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#

add_library( cla SHARED ${c_sources} ${c_headers} )
target_compile_options(cla PRIVATE -std=c++11)

