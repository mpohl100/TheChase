include_directories(../)

add_library( chase SHARED Chase.cpp Analysis.cpp TeamAnalysis.cpp GamePlan.cpp Player.cpp Util.cpp)
target_link_libraries( chase CONAN_PKG::range-v3)
target_link_libraries( chase evol)
target_include_directories(chase PUBLIC {CMAKE_SOURCE_DIR}/src/evol)