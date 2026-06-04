cmake_minimum_required(VERSION 3.20)

project(thync-cli)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/**" )
add_executable(thync)

target_include_directories(thync PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(thync PRIVATE ${SOURCES})

install(TARGETS thync RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")