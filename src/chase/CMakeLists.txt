include_directories(../)

find_package(Threads)
add_library( chase SHARED Chase.cpp Analysis.cpp TeamAnalysis.cpp TeamAnalysisPar.cpp TeamChase.cpp TeamGamePlan.cpp GamePlan.cpp Player.cpp Util.cpp Stochastics.cpp)
target_link_libraries( chase CONAN_PKG::range-v3)
target_link_libraries( chase evol)
target_link_libraries ( chase ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(chase PUBLIC {CMAKE_SOURCE_DIR}/src/evol)
target_include_directories(chase PUBLIC {CMAKE_SOURCE_DIR}/src/par)