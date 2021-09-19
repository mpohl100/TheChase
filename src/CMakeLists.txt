add_library( evol SHARED Rng.cpp )
target_include_directories(evol PUBLIC {CMAKE_SOURCE_DIR}/src)

add_library( chase SHARED Chase.cpp Analysis.cpp )
target_link_libraries( chase CONAN_PKG::range-v3)
target_link_libraries( chase evol)
target_include_directories(chase PUBLIC {CMAKE_SOURCE_DIR}/src)