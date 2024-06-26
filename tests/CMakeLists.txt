file(GLOB TESTS nomp-*.c)
foreach(test_src ${TESTS})
  string(REPLACE "${CMAKE_SOURCE_DIR}/tests/" "" temp ${test_src})
  string(REPLACE ".c" "" test_exe ${temp})
  add_executable(${test_exe} ${test_src})
  target_link_libraries(${test_exe} nomp)
  target_include_directories(${test_exe} PRIVATE ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/tests)
  target_compile_options(${test_exe} PRIVATE $<$<C_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>)
  if (HAS_ADDRESS_SANITIZER)
    target_compile_options(${test_exe} PRIVATE -fsanitize=address)
    target_link_options(${test_exe} PRIVATE -fsanitize=address)
  endif()
  install (TARGETS ${test_exe} RUNTIME DESTINATION
    ${CMAKE_INSTALL_PREFIX}/tests)
endforeach()

install(DIRECTORY ${CMAKE_SOURCE_DIR}/tests/ DESTINATION
  ${CMAKE_INSTALL_PREFIX}/tests FILES_MATCHING PATTERN "*.py")
