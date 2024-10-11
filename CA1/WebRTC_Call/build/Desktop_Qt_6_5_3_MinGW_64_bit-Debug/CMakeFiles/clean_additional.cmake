# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\appWebRTC_Call_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\appWebRTC_Call_autogen.dir\\ParseCache.txt"
  "appWebRTC_Call_autogen"
  )
endif()
