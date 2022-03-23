cmake_minimum_required(VERSION 3.7)

project(bjgen VERSION 0.0.3 LANGUAGES C CXX)
add_executable(bjgen)

add_subdirectory(libs/OpenXLSX)

file(GLOB sources "src/*.cpp")
file(GLOB includes "include/*.hpp")
include_directories(
	include
)
target_link_libraries(bjgen
	PRIVATE OpenXLSX::OpenXLSX
)

target_sources(bjgen PRIVATE ${sources} ${includes})
