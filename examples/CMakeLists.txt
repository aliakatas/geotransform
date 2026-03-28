get_filename_component(_GEOTRANSFORM_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

add_executable(example_geotransform_simple
  geotransform_simple.cxx
)

add_executable(example_geotransform_advanced
  geotransform_advanced.cxx
)

foreach(_tgt IN ITEMS example_geotransform_simple example_geotransform_advanced)
  target_include_directories(${_tgt} PRIVATE "${_GEOTRANSFORM_ROOT}/include")
  target_compile_features(${_tgt} PRIVATE cxx_std_17)
  if(MSVC)
    target_compile_definitions(${_tgt} PRIVATE _USE_MATH_DEFINES)
  endif()
endforeach()
