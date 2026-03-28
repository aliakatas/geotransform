# Included from the repository root via add_subdirectory(tests).
# The top-level CMakeLists.txt must include(CTest) before add_subdirectory(tests)
# so ctest discovers add_test() entries here.

get_filename_component(_GEOTRANSFORM_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

add_executable(test_geotransform_utilities
  test_geotransform_utilities.cxx
)

target_include_directories(test_geotransform_utilities PRIVATE
  "${_GEOTRANSFORM_ROOT}/include"
  "${_GEOTRANSFORM_ROOT}/extern/doctest"
)

target_compile_features(test_geotransform_utilities PRIVATE cxx_std_17)

if(MSVC)
  target_compile_definitions(test_geotransform_utilities PRIVATE _USE_MATH_DEFINES)
endif()

add_executable(test_geotransform
  test_geotransform.cxx
)

target_include_directories(test_geotransform PRIVATE
  "${_GEOTRANSFORM_ROOT}/include"
  "${_GEOTRANSFORM_ROOT}/extern/doctest"
)

target_compile_features(test_geotransform PRIVATE cxx_std_17)

if(MSVC)
  target_compile_definitions(test_geotransform PRIVATE _USE_MATH_DEFINES)
endif()

add_test(
  NAME test_geotransform_utilities
  COMMAND "$<TARGET_FILE:test_geotransform_utilities>"
)

add_test(
  NAME test_geotransform
  COMMAND "$<TARGET_FILE:test_geotransform>"
)
