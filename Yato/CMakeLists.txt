cmake_minimum_required (VERSION 3.15)

add_executable (Yato "Yato.cpp" "Yato.h")
set_target_properties(Yato PROPERTIES CXX_STANDARD 17)

find_package(yaml-cpp CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

target_link_libraries(Yato PRIVATE fmt::fmt fmt::fmt-header-only yaml-cpp)

if (UNIX)
	target_link_libraries(Yato PRIVATE stdc++fs)
endif (UNIX)