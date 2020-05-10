#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(HunapuSim)

cmake_policy(SET CMP0042 NEW)


# add the cla library
add_subdirectory(cla)

# add the WCDTank executable
add_subdirectory(WCDtank)

# add the elec executable
add_subdirectory(elec)

#----------------------------------------------------------------------------
# Copy all scripts to the build directory.
#
set(ROOT_SCRIPTS
    pulses.C
    pulses2.C
    pulses3.C
  )

foreach(_script ${elec_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/ROOT-Scripts/${_script}
    ${PROJECT_BINARY_DIR}/ROOT-Scripts/${_script}
    COPYONLY
    )
endforeach()