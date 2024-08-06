add_subdirectory(SwitchStr)

add_executable(${PROJECT_NAME}-print-version
  print-version.cpp
  )

target_link_libraries(${PROJECT_NAME}-print-version
  PRIVATE
  ${PROJECT_NAME}::${PROJECT_NAME}
  )

# TODO install
