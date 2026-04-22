# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\appProgettoTIPSIT_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\appProgettoTIPSIT_autogen.dir\\ParseCache.txt"
  "appProgettoTIPSIT_autogen"
  )
endif()
