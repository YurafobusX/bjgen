cmake_minimum_required(VERSION 3.7)

project(bjgen VERSION 0.0.1 LANGUAGES C CXX)
add_executable(bjgen)


file(GLOB sources "src/*.cpp")
file(GLOB includes "include/*.hpp")
include_directories(
	include
)

target_sources(bjgen PRIVATE ${sources} ${includes})
