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
