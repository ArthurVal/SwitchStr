cmake_minimum_required(VERSION 3.10)

project(SwitchStr
  VERSION 0.0.1
  DESCRIPTION "Something something something"
  LANGUAGES CXX
  )

include(cmake/DisableInSourceBuildDir.cmake)

include(CMakePrintHelpers)
include(GNUInstallDirs)

configure_file(
  include/${PROJECT_NAME}/Version.hpp.in
  include/${PROJECT_NAME}/Version.hpp
  @ONLY
  )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

install(
  DIRECTORY
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include/>
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  PATTERN "*.in" EXCLUDE
  PATTERN "*.h"
  )

include(cmake/TestingOptions.cmake)
if(${PROJECT_NAME}_ENABLE_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()
